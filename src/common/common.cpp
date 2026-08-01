//
// Created by ra1nbolt on 5/20/26.
//

#include "common.h"

namespace common {
    std::unique_ptr<utils::memory::module_t> target_lib;
    std::map<signatures::category, std::map<std::string, uintptr_t>> resolved_offsets;
    std::map<field_type, std::map<std::string, ptrdiff_t>> resolved_fields;

    uintptr_t find_address(const std::string &key) {
        static uintptr_t base = target_lib->get_base_address();

        for (const auto& [category, inner_map] : resolved_offsets) {
            auto it = inner_map.find(key);
            if (it != inner_map.end()) return it->second + base;
        }

        return 0;
    }

    ptrdiff_t find_field(field_type type, const std::string &key) {
        auto outer_it = resolved_fields.find(type);
        if (outer_it != resolved_fields.end()) {
            auto inner_it = outer_it->second.find(key);
            if (inner_it != outer_it->second.end()) {
                return inner_it->second;
            }
        }

        return 0;
    }
}