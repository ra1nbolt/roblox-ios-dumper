//
// Created by ra1nbolt on 5/20/26.
//

#include "job.h"
//
// Created by ra1nbolt on 5/20/26.
//

#include "job.h"

namespace dumper::stages {
    utils::logger &job::l = utils::logger::get_instance();
    utils::disassembler job::disasm;

    ptrdiff_t job::find_job_name_offset() {
        uintptr_t addr = common::find_address("datamodeljob");
        if (!addr) {
            l.log(utils::log_level::error, "unable to find job name offset: datamodeljob is not resolved");
            return 0;
        }

        if (!disasm.status()) return 0;

        cs_insn* insns = nullptr;
        size_t count = disasm.disassemble(reinterpret_cast<const uint8_t *>(addr), 120, addr, &insns);

        if (count == 0 || !insns) {
            l.log(utils::log_level::error, "unable to find job name offset: disassembly failed");
            return 0;
        }

        ptrdiff_t resolved_offset = 0;
        bool success = false;

        for (size_t i = 0; i < count; i++) {
            if (insns[i].id == ARM64_INS_BL) {
                arm64_reg track_reg = ARM64_REG_X0;

                for (size_t j = i; j > 0 && j >= i - 4; j--) {
                    const cs_insn& prev_insn = insns[j - 1];
                    if (!prev_insn.detail) continue;

                    const auto& arm64 = prev_insn.detail->arm64;

                    if (prev_insn.id == ARM64_INS_MOV) {
                        if (arm64.op_count == 2 &&
                            arm64.operands[0].type == ARM64_OP_REG && arm64.operands[0].reg == track_reg &&
                            arm64.operands[1].type == ARM64_OP_REG) {
                            track_reg = arm64.operands[1].reg;
                        }
                    }

                    if (prev_insn.id == ARM64_INS_ADD) {
                        if (arm64.op_count == 3 &&
                            arm64.operands[0].type == ARM64_OP_REG && arm64.operands[0].reg == track_reg &&
                            arm64.operands[2].type == ARM64_OP_IMM) {

                            resolved_offset = static_cast<ptrdiff_t>(arm64.operands[2].imm);
                            success = true;

                            break;
                        }
                    }
                }
            }
            if (success) break;
        }

        disasm.free_insn(insns, count);

        if (!success) {
            l.log(utils::log_level::error, "unable to find job name offset: failed to parse string initialization");
            return 0;
        }

        return resolved_offset;
    }

    void job::dump() {
        std::map<std::string, ptrdiff_t> &fmap = common::resolved_fields[common::field_type::job];

        ptrdiff_t name_offset = find_job_name_offset();

        fmap["name"] = name_offset;

        for (const auto &entry : fmap) {
            l.log(utils::log_level::info, fmt::format("job::{} -> {:#x}", entry.first, entry.second));
        }
    }
}