//
// Created by ra1nbolt on 5/11/26.
//

#include "logger.h"

#include <Foundation/Foundation.h>
#include <chrono>
#include <iomanip>

namespace utils {
    logger &logger::get_instance() {
        static logger instance;
        return instance;
    }

    logger::logger() {
        std::time_t now = std::time(nullptr);
        char buf[20];

        std::string base = get_documents_path() + "/log_";
        if (std::strftime(buf, sizeof(buf), "%Y-%m-%d_%H:%M:%S", std::localtime(&now))) {
            std::string s(buf);

            std::string path = base + s + ".txt";
            log_file.open(path, std::ios::app);
        } else {
            std::string path = base + "time_error" + ".txt";
            log_file.open(path, std::ios::app);
        }
    }

    logger::~logger() {
        if (log_file.is_open()) {
            log_file.close();
        }
    }

    std::string logger::get_documents_path() {
        @autoreleasepool {
            NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
            NSString *documents_directory = [paths firstObject];
            return [documents_directory UTF8String];
        }
    }

    std::string logger::level_to_string(log_level level) {
        switch (level) {
            case log_level::info:    return "INFO";
            case log_level::warning: return "WARN";
            case log_level::error:   return "ERR ";
        }
    }

    void logger::log(log_level level, const std::string& message) {
        std::lock_guard<std::mutex> lock(log_mutex);
        
        if (!log_file.is_open()) return;

        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        log_file << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S")
                 << "." << std::setfill('0') << std::setw(3) << ms.count()
                 << " [" << level_to_string(level) << "] " 
                 << message << std::endl;
    }
}