
#include "disassembler.h"

namespace utils {
    disassembler::disassembler() {
        utils::logger &l = utils::logger::get_instance();

        int maj, min;
        cs_version(&maj, &min);
        l.log(log_level::info, "initializing disasm. capstone ver. -> " + std::to_string(maj) + "." + std::to_string(min));

        cs_err cs_ret = cs_open(CS_ARCH_ARM64, CS_MODE_ARM, &handle);

        if (cs_ret == CS_ERR_OK) {
            cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);
            is_valid = true;
        }

        l.log(log_level::info, std::string("disasm init status: ") + cs_strerror(cs_ret));
    }

    disassembler::~disassembler() {
        if (is_valid) {
            cs_close(&handle);
        }
    }

    bool disassembler::status() const {
        return is_valid;
    }

    size_t disassembler::disassemble(const uint8_t *code, size_t size, uint64_t address, cs_insn **insn) {
        return cs_disasm(handle, code, size, address, 0, insn);
    }

    void disassembler::free_insn(cs_insn *insn, size_t count) {
        cs_free(insn, count);
    }

    uintptr_t disassembler::resolve_adrp_add(const cs_insn *adrp, const cs_insn *add) {
        if (!adrp || !add || !adrp->detail || !add->detail ||
            adrp->id != ARM64_INS_ADRP || add->id != ARM64_INS_ADD) {
            return 0;
        }

        uintptr_t target = 0;

        for (int i = 0; i < adrp->detail->arm64.op_count; i++) {
            if (adrp->detail->arm64.operands[i].type == ARM64_OP_IMM) {
                target = adrp->detail->arm64.operands[i].imm;
            }
        }

        for (int i = 0; i < add->detail->arm64.op_count; i++) {
            if (add->detail->arm64.operands[i].type == ARM64_OP_IMM) {
                target += add->detail->arm64.operands[i].imm;
            }
        }

        return target;
    }

    uintptr_t disassembler::resolve_bl(uintptr_t address) {
        if (!is_valid) return 0;

        const uint8_t* code_ptr = reinterpret_cast<const uint8_t*>(address);
        cs_insn* insn = nullptr;
        uintptr_t target_address = 0;

        size_t count = cs_disasm(handle, code_ptr, 4, address, 1, &insn);

        if (count > 0) {
            if (insn[0].id == ARM64_INS_BL && insn[0].detail) {
                const cs_arm64& arm64 = insn[0].detail->arm64;

                if (arm64.op_count == 1 && arm64.operands[0].type == ARM64_OP_IMM) target_address = arm64.operands[0].imm;
            }
            cs_free(insn, count);
        }

        return target_address;
    }

    uintptr_t disassembler::resolve_ldr(uintptr_t address, size_t depth) {
        if (!is_valid) return 0;

        const uint8_t* ldr_code_ptr = reinterpret_cast<const uint8_t*>(address);
        cs_insn* insn = nullptr;

        arm64_reg base_reg = ARM64_REG_INVALID;
        int64_t offset = 0;
        bool ldr_parsed = false;

        size_t count = cs_disasm(handle, ldr_code_ptr, 4, address, 1, &insn);
        if (count > 0) {
            if (insn[0].id == ARM64_INS_LDR && insn[0].detail && insn[0].detail->arm64.op_count == 2) {
                const cs_arm64_op& mem_op = insn[0].detail->arm64.operands[1];
                if (mem_op.type == ARM64_OP_MEM) {
                    base_reg = mem_op.mem.base;
                    offset = mem_op.mem.disp;
                    ldr_parsed = true;
                }
            }
            cs_free(insn, count);
        }

        if (!ldr_parsed || base_reg == ARM64_REG_INVALID) return 0;

        for (size_t i = 1; i <= depth; i++) {
            uintptr_t current_addr = address - (i * 4);
            const uint8_t* current_code_ptr = reinterpret_cast<const uint8_t*>(current_addr);

            cs_insn* prev_insn = nullptr;
            size_t prev_count = cs_disasm(handle, current_code_ptr, 4, current_addr, 1, &prev_insn);

            if (prev_count > 0) {
                if (prev_insn[0].id == ARM64_INS_ADRP && prev_insn[0].detail) {
                    const cs_arm64& arm64 = prev_insn[0].detail->arm64;

                    if (arm64.op_count > 0 && arm64.operands[0].type == ARM64_OP_REG && arm64.operands[0].reg == base_reg) {
                        uintptr_t adrp_page = 0;

                        if (arm64.op_count > 1 && arm64.operands[1].type == ARM64_OP_IMM) {
                            adrp_page = arm64.operands[1].imm;
                        }

                        cs_free(prev_insn, prev_count);
                        return adrp_page + offset;
                    }
                }
                cs_free(prev_insn, prev_count);
            }
        }

        return 0;
    }

    uintptr_t disassembler::resolve_adrl(uintptr_t address) {
        if (!is_valid) return 0;

        const uint8_t* code_ptr = reinterpret_cast<const uint8_t*>(address);
        cs_insn* insn = nullptr;
        uintptr_t target_address = 0;

        size_t count = cs_disasm(handle, code_ptr, 8, address, 2, &insn);

        if (count == 2) {
            target_address = resolve_adrp_add(&insn[0], &insn[1]);
            cs_free(insn, count);
        } else if (count > 0) {
            cs_free(insn, count);
        }

        return target_address;
    }

    bool disassembler::writes_to_reg(const cs_insn *insn, arm64_reg reg) {
        if (!insn || !insn->detail) return false;

        if (insn->detail->arm64.op_count > 0) {
            cs_arm64_op op = insn->detail->arm64.operands[0];
            if (op.type == ARM64_OP_REG && op.reg == reg) {
                return true;
            }
        }

        for (int i = 0; i < insn->detail->regs_write_count; i++) {
            if (insn->detail->regs_write[i] == reg) {
                return true;
            }
        }

        return false;
    }
}