#include "src/common/common.h"
#include "dumper/dumper.h"
#include "dumper/stages/lstate/lstate.h"
#include "utils/logger/logger.h"
#include "utils/dump_exporter/dump_exporter.h"
#include "src/signatures/signatures.h"

#include <fmt/format.h>
#include <filesystem>
#include <fstream>
#include <map>
#include <thread>
#include <string_view>

namespace fs = std::filesystem;

void main_thread() {
    utils::logger& l = utils::logger::get_instance();

    l.log(utils::log_level::info, "roblox dumper ios | https://github.com/ra1nbolt/roblox-dumper-ios");
    l.log(utils::log_level::info, fmt::format("git hash: {}", GIT_HASH));
    l.log(utils::log_level::info, fmt::format("build type: {}", IS_DEVBUILD ? "developer" : "release"));
    l.log(utils::log_level::info, fmt::format("c++ version: {}", __cplusplus));

    common::target_lib = utils::memory::find_lib("RobloxLib");
    if (!common::target_lib) {
        l.log(utils::log_level::info, "roblox lib not found");
        return;
    }

    l.log(utils::log_level::info, "dumping client info");

    auto ci = dumper::dump_client_info();
    if (!ci) {
        l.log(utils::log_level::info, "unable to dump client info");
        return;
    }

    l.log(utils::log_level::info, fmt::format("client info:\n  type: {}\n  version: {}\n  git hash: {}", ci->product, ci->app_version, ci->git_hash));

    dumper::dump_offsets();

    fs::path documents_dir = utils::logger::get_documents_path();
    fs::path base_path = documents_dir / "dump";
    fs::path header_path = base_path / fmt::format("dump_{}.h", ci->app_version);

    fs::create_directories(base_path);
    if (!fs::exists(base_path)) {
        l.log(utils::log_level::error, "unable to create dump dir");
        return;
    }

    utils::dump_exporter exporter(base_path, ci->product, ci->app_version, ci->git_hash);

    exporter.write_header(common::resolved_offsets);
    exporter.write_json(common::resolved_offsets);

    dumper::dump_fields();
}

entrypoint init() {
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0), ^{ main_thread(); });
}