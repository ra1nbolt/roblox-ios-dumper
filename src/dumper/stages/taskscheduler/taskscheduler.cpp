//
// Created by ra1nbolt on 5/20/26.
//

#include "taskscheduler.h"

namespace dumper::stages {
    utils::logger &taskscheduler::l = utils::logger::get_instance();
    utils::disassembler taskscheduler::disasm;

    std::pair<ptrdiff_t, ptrdiff_t> taskscheduler::find_jobs_offsets() {
        uintptr_t addr = common::find_address("find_jobs");
        if (!addr) {
            l.log(utils::log_level::error, "unable to find jobs offset: find_jobs is not resolved");
            return {0, 0};
        }

        if (!disasm.status()) return {};

        cs_insn* insn = nullptr;
        size_t count = disasm.disassemble(reinterpret_cast<uint8_t *>(addr), 160, addr, &insn);

        std::pair<ptrdiff_t, ptrdiff_t> ret;

        for (size_t i = 0; i < count; i++) {
            if (insn[i].id == ARM64_INS_LDP) {
                cs_arm64* arm64 = &insn[i].detail->arm64;

                if (arm64->op_count == 3 && arm64->operands[2].type == ARM64_OP_MEM) {
                    auto mem = arm64->operands[2].mem;

                    if (mem.disp != 0) {
                        ret = {mem.disp, mem.disp + 8};
                        break;
                    }
                }
            }
        }

        if (insn) disasm.free_insn(insn, count);

        return ret;
    }

    void taskscheduler::dump() {
        std::map<std::string, ptrdiff_t> &fmap = common::resolved_fields[common::field_type::taskscheduler];

        std::pair<ptrdiff_t, ptrdiff_t> jobs_offsets = find_jobs_offsets();

        fmap["jobs_start"] = jobs_offsets.first;
        fmap["jobs_end"] = jobs_offsets.second;

        for (const auto &entry : fmap) {
            l.log(utils::log_level::info, fmt::format("taskscheduler::{} -> {:#x}", entry.first, entry.second));
        }
    }
}