//
// Created by ra1nbolt on 5/20/26.
//

#include "dump_exporter.h"

namespace utils {
    utils::logger &l = utils::logger::get_instance();

    void dump_exporter::write_header(const std::map<signatures::category, std::map<std::string, uintptr_t>> &offsets)  {
        fs::path header_path = base_path / fmt::format("dump_{}.h", app_version);

        l.log(utils::log_level::info, fmt::format("saving offsets header to {}", header_path.string()));

        std::ofstream header_file(header_path);
        if (!header_file.is_open()) {
            l.log(utils::log_level::error, "unable to write offsets header");
            return;
        }

        header_file << "// roblox dumper ios | https://github.com/ra1nbolt/roblox-dumper-ios\n";
        header_file << fmt::format("// dumped product: {} {} (git hash: {})\n\n", product, app_version, git_hash);

        header_file << "#include <cstdint>\n\n";

        header_file << "namespace offsets {\n";

        for (auto it = offsets.begin(); it != offsets.end(); ++it) {
            const auto &[cat, cat_offsets] = *it;
            if (cat == signatures::category::none) continue;

            std::string_view cat_str = signatures::cat2str(cat);

            header_file << fmt::format("    namespace {} {{\n", cat_str);
            for (const auto &[offset_name, offset_val]: cat_offsets) {
                header_file << fmt::format("        const uintptr_t {} = {:#x};\n", offset_name, offset_val);
            }
            header_file << fmt::format("    }} // namespace {}\n", cat_str);

            if (std::next(it) != offsets.end()) {
                header_file << "\n";
            }
        }
        header_file << "} // namespace offsets\n";
    }

    void dump_exporter::write_json(const std::map<signatures::category, std::map<std::string, uintptr_t>> &offsets) {
        fs::path json_path = base_path / fmt::format("dump_{}.json", app_version);

        l.log(utils::log_level::info, fmt::format("saving offsets json to {}", json_path.string()));

        json j;
        j["metadata"] = {
                {"product",   product},
                {"version",   app_version},
                {"git_hash",  git_hash},
        };

        // Формируем JSON структуру
        for (const auto &[cat, cat_offsets]: offsets) {
            if (cat == signatures::category::none) continue;

            std::string cat_str(signatures::cat2str(cat));
            for (const auto &[offset_name, offset_val]: cat_offsets) {
                // В стандарте JSON нет шестнадцатеричных чисел, поэтому они запишутся в десятичном виде (e.g. 12345).
                // IDA Pro это прекрасно понимает.
                j["offsets"][cat_str][offset_name] = offset_val;
            }
        }

        std::ofstream json_file(json_path);
        if (!json_file.is_open()) {
            l.log(utils::log_level::error, "unable to write offsets json");
            return;
        }

        // Пишем с отступами в 4 пробела для красивого форматирования
        json_file << j.dump(4) << "\n";
    }
}