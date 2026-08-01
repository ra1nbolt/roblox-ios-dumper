//
// Created by ra1nbolt on 5/12/26.
//

#include "dumper.h"

namespace dumper {
    utils::logger &l = utils::logger::get_instance();

    std::unique_ptr<client_info> dump_client_info() {
        l.log(utils::log_level::info, "lib ptr -> " + std::to_string(common::target_lib->get_base_address()));

        auto func_export = common::target_lib->find_export("_ZN3RBX12CrashManager14CrashpadFacade13checkInitArgsEv");
        if (!func_export) return nullptr;

        uintptr_t func_addr = func_export->memory_address;
        l.log(utils::log_level::info, fmt::format("checkInitArgs -> {:#x}", func_addr));

        utils::disassembler dis;
        if (!dis.status()) return nullptr;

        cs_insn* insns;
        size_t count = dis.disassemble(
                reinterpret_cast<uint8_t*>(func_addr),
                0x800,
                func_addr,
                &insns
        );

        if (count == 0) return nullptr;

        auto metadata = std::make_unique<client_info>();

        for (size_t i = 0; i < count - 1; i++) {
            if (insns[i].id == ARM64_INS_ADRP && insns[i + 1].id == ARM64_INS_ADD) {
                if (!dis.writes_to_reg(&insns[i + 1], ARM64_REG_X1)) continue;

                uint64_t addr = dis.resolve_adrp_add(&insns[i], &insns[i + 1]);

                if (addr < 0x100000000) continue;

                std::string key = reinterpret_cast<const char*>(addr);

                auto get_val = [&](std::string& output) {
                    for (size_t j = i + 1; j < i + 30 && j < count - 1; j++) {
                        if (insns[j].id == ARM64_INS_ADRP && insns[j+1].id == ARM64_INS_ADD) {
                            if (dis.writes_to_reg(&insns[j+1], ARM64_REG_X1)) {
                                uint64_t v_addr = dis.resolve_adrp_add(&insns[j], &insns[j+1]);
                                if (v_addr >= 0x100000000) {
                                    output = reinterpret_cast<const char*>(v_addr);
                                    return true;
                                }
                            }
                        }
                    }
                    return false;
                };

                if (key == "AppVersion") get_val(metadata->app_version);
                else if (key == "RobloxGitHash") get_val(metadata->git_hash);
                else if (key == "RobloxProduct") get_val(metadata->product);
            }

            if (!metadata->app_version.empty() &&
                !metadata->git_hash.empty() &&
                !metadata->product.empty()) {
                break;
            }
        }

        dis.free_insn(insns, count);
        return metadata;
    }

    void dump_fields() {
        stages::taskscheduler::dump();
        stages::instance::dump();
        stages::job::dump();
        stages::lstate::dump();
    }

    void dump_offsets() {
        uintptr_t base = common::target_lib->get_base_address();
        utils::disassembler disasm;

        auto start = std::chrono::steady_clock::now();

        for (const auto& [sig, info] : signatures::list) {
            uintptr_t ptr = common::target_lib->find_signature(info.pattern);

            if (static_cast<intptr_t>(ptr) <= base) {
                l.log(utils::log_level::error, fmt::format("unable to resolve {} address", sig.name));
                continue;
            }

            uintptr_t addr = 0;
            switch (info.resolve) {
                case signatures::resolve_type::none:
                    addr = ptr - base;
                    break;
                case signatures::resolve_type::bl:
                    addr = disasm.resolve_bl(ptr) - base;
                    break;
                case signatures::resolve_type::ldr:
                    addr = disasm.resolve_ldr(ptr) - base;
                    break;
                case signatures::resolve_type::adrl:
                    addr = disasm.resolve_adrl(ptr) - base;
                    break;
            }

            l.log(utils::log_level::info, fmt::format("{} -> {:#x}", sig.name, addr));
            common::resolved_offsets[sig.cat][sig.name] = addr;
        }

        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        l.log(utils::log_level::info, fmt::format("took {:.2f}s to dump", elapsed.count()));
    }
}