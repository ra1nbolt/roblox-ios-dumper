//
// Created by ra1nbolt on 5/20/26.
//

#pragma once

#include <cstddef>
#include "src/utils/disassembler/disassembler.h"
#include "src/utils/logger/logger.h"
#include "src/common/common.h"

namespace dumper::stages {
    class lstate {
    private:
        struct thread_offsets {
            ptrdiff_t stacksize = -1;

            bool valid() const {
                return stacksize != -1;
            }
        };

        static utils::disassembler disasm;
        static utils::logger &l;

        static ptrdiff_t get_userdata_offset();
        static std::pair<ptrdiff_t, ptrdiff_t> get_top_base_offsets();
        static thread_offsets parse_thread();

    public:
        static void dump();
    };
}