//
// Created by ra1nbolt on 5/12/26.
//

#pragma once

#include <memory>
#include <fmt/format.h>
#include "src/utils/disassembler/disassembler.h"
#include "src/utils/memory/memory.h"
#include "src/utils/logger/logger.h"
#include "src/common/common.h"
#include "src/dumper/stages/taskscheduler/taskscheduler.h"
#include "src/dumper/stages/instance/instance.h"
#include "src/dumper/stages/lstate/lstate.h"
#include "src/dumper/stages/job/job.h"

namespace dumper {
    extern utils::logger &l;

    struct client_info {
        std::string app_version;
        std::string git_hash;
        std::string product;
    };

    std::unique_ptr<client_info> dump_client_info();
    void dump_fields();
    void dump_offsets();
}