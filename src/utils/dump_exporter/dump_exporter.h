//
// Created by ra1nbolt on 5/20/26.
//

#pragma once

#include <string>
#include <filesystem>
#include <json.hpp>
#include <fmt/format.h>
#include "src/signatures/signatures.h"
#include "../logger/logger.h"

namespace fs = std::filesystem;
using namespace nlohmann;

namespace utils {
    extern utils::logger &l;

    class dump_exporter {
    private:
        fs::path base_path;
        std::string product;
        std::string app_version;
        std::string git_hash;

    public:
        dump_exporter(fs::path path, std::string prod, std::string ver, std::string hash)
                : base_path(std::move(path)), product(std::move(prod)), app_version(std::move(ver)), git_hash(std::move(hash)) {}

        void write_header(const std::map<signatures::category, std::map<std::string, uintptr_t>> &offsets);
        void write_json(const std::map<signatures::category, std::map<std::string, uintptr_t>> &offsets);
    };
}