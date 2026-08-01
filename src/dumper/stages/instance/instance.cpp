//
// Created by ra1nbolt on 5/20/26.
//

#include "instance.h"

namespace dumper::stages {
    utils::logger &instance::l = utils::logger::get_instance();
    utils::disassembler instance::disasm;

    ptrdiff_t instance::find_parent_offset() {
        uintptr_t addr = common::find_address("get_root_ancestor");
        if (!addr) {
            l.log(utils::log_level::error, "unable to find parent offset: get_root_ancestor is not resolved");
            return 0;
        }

        if (!disasm.status()) return 0;

        cs_insn* insns = nullptr;
        size_t count = disasm.disassemble(reinterpret_cast<const uint8_t *>(addr), 160, addr, &insns);

        if (count == 0 || !insns) {
            l.log(utils::log_level::error, "unable to find parent offset: disassembly failed");
            return 0;
        }

        arm64_reg track_reg = ARM64_REG_X0;
        ptrdiff_t resolved_offset = 0;
        bool success = false;

        for (size_t i = 0; i < count; i++) {
            const cs_insn& insn = insns[i];
            if (!insn.detail) continue;

            const auto& arm64 = insn.detail->arm64;

            if (insn.id == ARM64_INS_MOV) {
                if (arm64.op_count == 2 &&
                    arm64.operands[0].type == ARM64_OP_REG &&
                    arm64.operands[1].type == ARM64_OP_REG &&
                    arm64.operands[1].reg == track_reg) {

                    track_reg = arm64.operands[0].reg;
                    continue;
                }
            }

            if (insn.id == ARM64_INS_LDR || insn.id == ARM64_INS_LDRB ||
                insn.id == ARM64_INS_LDRH || insn.id == ARM64_INS_STR) {

                if (arm64.op_count >= 2 && arm64.operands[1].type == ARM64_OP_MEM) {
                    const auto& mem = arm64.operands[1].mem;

                    if (mem.base == track_reg) {
                        resolved_offset = static_cast<ptrdiff_t>(mem.disp);
                        success = true;

                        l.log(utils::log_level::info, "successfully resolved parent offset: 0x" +
                                                      std::to_string(resolved_offset) + " at instruction " + insn.mnemonic +
                                                      " on address 0x" + std::to_string(insn.address));
                        break;
                    }
                }
            }
        }

        disasm.free_insn(insns, count);

        if (!success) {
            l.log(utils::log_level::error, "unable to find parent offset: memory access to parameter not found");
            return 0;
        }

        return resolved_offset;
    }

    void instance::dump() {
        std::map<std::string, ptrdiff_t> &fmap = common::resolved_fields[common::field_type::instance];

        ptrdiff_t parent_offset = find_parent_offset();
        //ptrdiff_t name_offset = find_name_offset();

        //fmap["name"] = name_offset;
        fmap["parent"] = parent_offset;

        for (const auto &entry : fmap) {
            l.log(utils::log_level::info, fmt::format("job::{} -> {:#x}", entry.first, entry.second));
        }
    }
}