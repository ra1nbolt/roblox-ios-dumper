//
// Created by ra1nbolt on 5/20/26.
//

#include "lstate.h"

namespace dumper::stages {
    utils::logger &lstate::l = utils::logger::get_instance();
    utils::disassembler lstate::disasm;

    ptrdiff_t lstate::get_userdata_offset() {
        uintptr_t addr = common::find_address("get_extra_space");
        if (!addr) {
            l.log(utils::log_level::error, "unable to dump userdata offset: get_extra_space is not resolved");
            return 0;
        }

        if (disasm.status()) {
            cs_insn* insn = nullptr;

            size_t count = disasm.disassemble(
                    reinterpret_cast<const uint8_t*>(addr),
                    4,
                    addr,
                    &insn
            );

            if (count > 0) {
                // Проверяем, что это LDR и у неё есть распарсенные детали (благодаря CS_OPT_DETAIL в твоем конструкторе)
                if (insn[0].id == ARM64_INS_LDR && insn[0].detail && insn[0].detail->arm64.op_count == 2) {

                    // Берем второй операнд (источник), который отвечает за обращение к памяти
                    const cs_arm64_op& src_op = insn[0].detail->arm64.operands[1];

                    if (src_op.type == ARM64_OP_MEM) {
                        int64_t userdata_offset = src_op.mem.disp;

                        l.log(utils::log_level::info, fmt::format("found lua_State::userdata offset: {:#x}", userdata_offset));

                        return userdata_offset;
                    }
                }

                disasm.free_insn(insn, count);
            }
        }

        return 0;
    }

    std::pair<ptrdiff_t, ptrdiff_t> lstate::get_top_base_offsets() {
        uintptr_t addr = common::find_address("lua_gettop");
        if (!addr) {
            l.log(utils::log_level::error, "unable to dump top/base offsets: lua_gettop is not resolved");
            return {0, 0};
        }

        if (disasm.status()) {
            cs_insn* insn = nullptr;

            size_t count = disasm.disassemble(
                    reinterpret_cast<const uint8_t*>(addr),
                    16,
                    addr,
                    &insn
            );

            ptrdiff_t top_offset = 0;
            ptrdiff_t base_offset = 0;

            if (count >= 2) {
                if (insn[0].id == ARM64_INS_LDR && insn[0].detail && insn[0].detail->arm64.op_count == 2) {
                    const cs_arm64_op& src_op = insn[0].detail->arm64.operands[1];
                    if (src_op.type == ARM64_OP_MEM && src_op.mem.base == ARM64_REG_X0) {
                        top_offset = src_op.mem.disp;
                    }
                }

                if (insn[1].id == ARM64_INS_LDR && insn[1].detail && insn[1].detail->arm64.op_count == 2) {
                    const cs_arm64_op& src_op = insn[1].detail->arm64.operands[1];
                    if (src_op.type == ARM64_OP_MEM && src_op.mem.base == ARM64_REG_X0) {
                        base_offset = src_op.mem.disp;
                    }
                }

                if (top_offset && base_offset) {
                    l.log(utils::log_level::info, fmt::format("found lua_State::top offset: {:#x}", top_offset));
                    l.log(utils::log_level::info, fmt::format("found lua_State::base offset: {:#x}", base_offset));
                } else {
                    l.log(utils::log_level::error, "failed to parse top/base offsets from gettop");
                }
            }

            if (count > 0) {
                disasm.free_insn(insn, count);
            }

            return {top_offset, base_offset};
        }

        return {0, 0};
    }

    lstate::thread_offsets lstate::parse_thread() {
        uintptr_t nt = common::find_address("lua_newthread");
        if (!nt) {
            l.log(utils::log_level::error, "unable to parse lua thread: lua_newthread is not resolved");
            return {};
        }

        typedef uintptr_t(* newthread_t)(uintptr_t L);
        auto newthread = (newthread_t)nt;

        uintptr_t ts = common::find_address("taskscheduler");
        ptrdiff_t js = common::find_field(common::taskscheduler, "jobs_start");
        ptrdiff_t je = common::find_field(common::taskscheduler, "jobs_end");
        ptrdiff_t kn = common::find_field(common::job, "name");

        (void)newthread;
        (void)(ts + js + je + kn);

        return {};
    }

    void lstate::dump() {
        std::map<std::string, ptrdiff_t> &fmap = common::resolved_fields[common::field_type::lstate];

        ptrdiff_t ud_offset = get_userdata_offset();
        std::pair<ptrdiff_t, ptrdiff_t> tb_offset = get_top_base_offsets();

        // these 7 fields should be unchanged (and im too lazy to find out how to dump them)
        fmap["tt"] = 0x0;
        fmap["marked"] = 0x1;
        fmap["memcat"] = 0x2;
        fmap["status"] = 0x3;
        fmap["activememcat"] = 0x4;
        fmap["isactive"] = 0x5;
        fmap["singlestep"] = 0x6;
        fmap["_pad_0x1"] = 0x7;

        fmap["userdata"] = ud_offset;
        fmap["top"] = tb_offset.first;
        fmap["base"] = tb_offset.second;

        for (const auto &entry : fmap) {
            l.log(utils::log_level::info, fmt::format("ls::{} -> {:#x}", entry.first, entry.second));
        }
    }
}
