//
// Created by ra1nbolt on 5/11/26.
//

#pragma once

#include <map>
#include <cstddef>
#include "src/utils/memory/memory.h"
#include "src/signatures/signatures.h"
#include "src/dumper/dumper.h"

#define entrypoint __attribute__((constructor)) void

namespace common {
    enum field_type {
        lstate,
        taskscheduler,
        job,
        instance
    };

    extern std::unique_ptr<utils::memory::module_t> target_lib;
    extern std::map<signatures::category, std::map<std::string, uintptr_t>> resolved_offsets;
    extern std::map<field_type, std::map<std::string, ptrdiff_t>> resolved_fields;

    uintptr_t find_address(const std::string &key);
    ptrdiff_t find_field(field_type type, const std::string &key);
}