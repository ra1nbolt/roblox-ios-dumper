//
// Created by ra1nbolt on 5/20/26.
//

#pragma once

#include <cstddef>
#include <utility>
#include "src/utils/disassembler/disassembler.h"
#include "src/common/common.h"

namespace dumper::stages {
    class job {
    private:
        static utils::disassembler disasm;
        static utils::logger &l;

        static ptrdiff_t find_job_name_offset();

    public:
        static void dump();
    };
}