//
// Created by ra1nbolt on 5/12/26.
//

#include "memory.h"

namespace utils {
    namespace memory {
        module_t::module_t(void *dl_handle, int index, std::string dl_path, uintptr_t base)
                : handle(dl_handle), index(index), path(std::move(dl_path)), base_address(base) {}

        module_t::~module_t() {
            if (handle) {
                dlclose(handle);
            }
        }

        std::unique_ptr<export_info_t> module_t::find_export(const std::string &symbol_name) const {
            if (!handle) return nullptr;

            // dlsym ищет адрес символа в конкретном дескрипторе библиотеки
            void *addr = dlsym(handle, symbol_name.c_str());
            if (!addr) {
                return nullptr;
            }

            return std::make_unique<export_info_t>(reinterpret_cast<uintptr_t>(addr), symbol_name);
        }

        std::unique_ptr<memory_range_t> module_t::find_segment(const std::string &seg_name) const {
            const auto *header = (const struct mach_header_64*)_dyld_get_image_header(this->index);
            size_t size = 0;
            auto start = (uintptr_t)getsegmentdata(header, seg_name.c_str(), &size);

            return size > 0x4000 && start != 0 ? std::make_unique<memory_range_t>(start, size) : nullptr;
        }

        uintptr_t module_t::find_signature(const std::string &pattern) const {
            auto mem_range = find_segment("__TEXT");
            if (!mem_range) return 0;

            compiled_pattern_t pat = compile_pattern(pattern.c_str());
            if (!pat.length) return 0;

            const uint8_t* scan_begin = reinterpret_cast<const uint8_t*>(mem_range->start );
            const uint8_t* scan_end = scan_begin + mem_range->size - pat.length;

            constexpr size_t THREAD_COUNT = 4;

            const size_t total_size = scan_end - scan_begin;
            const size_t chunk_size = total_size / THREAD_COUNT;

            __block std::atomic<uintptr_t> result = 0;

            dispatch_queue_t queue =
                    dispatch_get_global_queue(
                            QOS_CLASS_USER_INITIATED,
                            0
                    );

            dispatch_apply(THREAD_COUNT, queue, ^(size_t thread_index) {
                if (result.load(std::memory_order_acquire)) return;

                const uint8_t* chunk_begin = scan_begin + thread_index * chunk_size;
                const uint8_t* chunk_end = (thread_index == THREAD_COUNT - 1) ? scan_end : chunk_begin + chunk_size + pat.length;

                const uint8_t* current = chunk_begin;
                while (current < chunk_end) {
                    // early exit
                    if (result.load(std::memory_order_relaxed)) return;

                    // memchr first-byte search
                    current = reinterpret_cast<const uint8_t*>(memchr(current, pat.first_byte, chunk_end - current));

                    if (!current) return;

                    // pattern begin
                    const uint8_t* candidate = current - pat.first_non_wildcard;

                    // bounds
                    if (candidate < scan_begin || candidate >= scan_end) {
                        ++current;
                        continue;
                    }

                    // compare
                    if (compare_simd(candidate, pat)) {
                        if (!result.load(std::memory_order_relaxed))
                            result.store(reinterpret_cast<uintptr_t>(candidate), std::memory_order_release);

                        return;
                    }

                    // BMH skip
                    current += pat.skip_table[current[pat.length - 1]];
                }
            });

            return result.load();
        }

        static inline uint8_t hex_to_byte(const char* str) {
            auto hex = [](char c) -> uint8_t {
                if (c >= '0' && c <= '9')
                    return c - '0';

                if (c >= 'A' && c <= 'F')
                    return c - 'A' + 10;

                if (c >= 'a' && c <= 'f')
                    return c - 'a' + 10;

                return 0;
            };

            return (hex(str[0]) << 4) | hex(str[1]);
        }

        compiled_pattern_t module_t::compile_pattern(const char* pattern) {
            compiled_pattern_t result;

            const char* current = pattern;

            while (*current) {
                if (*current == ' ') {
                    ++current;
                    continue;
                }

                // wildcard
                if (*current == '?') {
                    ++current;

                    if (*current == '?')
                        ++current;

                    result.bytes.push_back(0);
                    result.mask.push_back(0);

                    continue;
                }

                // byte
                result.bytes.push_back(hex_to_byte(current));
                result.mask.push_back(0xFF);

                current += 2;
            }

            result.length = result.bytes.size();

            // first non wildcard
            for (size_t i = 0; i < result.length; ++i) {
                if (result.mask[i]) {
                    result.first_non_wildcard = i;
                    result.first_byte = result.bytes[i];
                    break;
                }
            }

            // BMH skip table
            std::fill(std::begin(result.skip_table), std::end(result.skip_table), result.length);

            for (size_t i = 0; i < result.length - 1; ++i) {
                if (result.mask[i]) {
                    result.skip_table[result.bytes[i]] =
                            result.length - i - 1;
                }
            }

            return result;
        }

        inline bool module_t::compare_simd(const uint8_t* data, const compiled_pattern_t& pat) {
            size_t i = 0;

            // SIMD compare 16 bytes
            for (; i + 16 <= pat.length; i += 16) {
                uint8x16_t data_vec = vld1q_u8(data + i);
                uint8x16_t pattern_vec = vld1q_u8(pat.bytes.data() + i);
                uint8x16_t mask_vec = vld1q_u8(pat.mask.data() + i);

                // xor
                uint8x16_t diff = veorq_u8(data_vec, pattern_vec);

                // apply mask
                diff = vandq_u8(diff, mask_vec);

                // any mismatch?
                uint64x2_t pair = reinterpret_cast<uint64x2_t>(diff);

                if (vgetq_lane_u64(pair, 0) ||
                    vgetq_lane_u64(pair, 1)) {
                    return false;
                }
            }

            // tail compare
            for (; i < pat.length; ++i) {
                if (pat.mask[i] &&
                    data[i] != pat.bytes[i]) {
                    return false;
                }
            }

            return true;
        }

        std::unique_ptr<module_t> find_lib(const std::string &lib_name) {
            uint32_t count = _dyld_image_count();

            for (uint32_t i = 0; i < count; i++) {
                const char *name = _dyld_get_image_name(i);
                if (!name) continue;

                std::string current_path(name);
                // Проверяем, содержит ли путь имя нужной нам библиотеки
                if (current_path.find(lib_name) != std::string::npos) {
                    // Открываем библиотеку через dlopen для получения handle.
                    // RTLD_NOLOAD гарантирует, что мы не загружаем её заново, если она уже есть.
                    void *handle = dlopen(name, RTLD_LAZY | RTLD_NOLOAD);
                    if (!handle) continue;

                    uintptr_t base = _dyld_get_image_vmaddr_slide(i);
                    return std::make_unique<module_t>(handle, i, current_path, base);
                }
            }

            return nullptr;
        }
    }
}