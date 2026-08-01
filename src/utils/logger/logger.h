//
// Created by ra1nbolt on 5/11/26.
//

#pragma once

#include <string>
#include <fstream>
#include <mutex>

namespace utils {
    enum class log_level {
        info,
        warning,
        error
    };

    class logger {
    public:
        static logger &get_instance();

        void log(log_level level, const std::string& message);

        static std::string get_documents_path();

    private:
        logger();
        ~logger();

        std::ofstream log_file;
        std::mutex log_mutex;

        static std::string level_to_string(log_level level);
    };
}