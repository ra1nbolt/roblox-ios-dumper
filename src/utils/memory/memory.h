//
// Created by ra1nbolt on 5/12/26.
//

#pragma once

#include <algorithm>
#include <atomic>
#include <cstring>
#include <string>
#include <sstream>
#include <memory>
#include <vector>
#include <dlfcn.h>
#include <cstdio>
#include <cstdint>
#include <dlfcn.h>
#include <arm_neon.h>
#include <mach/mach.h>
#include <mach-o/dyld.h>
#include <mach-o/loader.h>
#include <mach-o/getsect.h>
#include <mach-o/compact_unwind_encoding.h>
#include <dispatch/dispatch.h>
#include "src/utils/logger/logger.h"
#include <fmt/format.h>

#if __has_feature(ptrauth_intrinsics)
#include <ptrauth.h>
#endif

namespace utils {
    namespace memory {
        struct export_info_t {
            uintptr_t memory_address;
            std::string symbol_name;

            explicit export_info_t(uintptr_t address, std::string name)
                    : memory_address(address), symbol_name(std::move(name)) {}
        };

        struct memory_range_t {
            uintptr_t start;
            size_t size;

            explicit memory_range_t(uintptr_t start, size_t size) : start(start), size(size) {}
        };

        struct compiled_pattern_t {
            std::vector<uint8_t> bytes;
            std::vector<uint8_t> mask;

            size_t length = 0;

            uint8_t first_byte = 0;
            size_t first_non_wildcard = 0;

            size_t skip_table[256];
        };

        class module_t {
        private:
            void *handle;
            int index;
            std::string path;
            uintptr_t base_address;

            static compiled_pattern_t compile_pattern(const char* pattern);
            static inline bool compare_simd(const uint8_t* data, const compiled_pattern_t& pat);

        public:
            module_t(void *dl_handle, int index, std::string dl_path, uintptr_t base);

            ~module_t();

            module_t(const module_t &) = delete;

            module_t &operator=(const module_t &) = delete;

            [[nodiscard]] std::unique_ptr<export_info_t> find_export(const std::string &symbol_name) const;
            [[nodiscard]] std::unique_ptr<memory_range_t> find_segment(const std::string &seg_name) const;

            [[nodiscard]] uintptr_t find_signature(const std::string& pattern) const;

            [[nodiscard]] uintptr_t get_base_address() const { return base_address; }
        };

        std::unique_ptr<module_t> find_lib(const std::string &lib_name);
    }
}