#pragma once

#include "../utils/files.hpp"
#include "fmt/format.h"
#include "spdlog/common.h"
#include "spdlog/logger.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/spdlog.h"
#include <memory>
#include <string>

namespace yess::log
{
inline std::shared_ptr<spdlog::logger> file_logger;
inline bool only_file;

template <typename... Types> void info(std::string format, Types... args)
{
    if (file_logger) {
        file_logger->info(format, args...);
        if (only_file)
            return;
    }

    spdlog::info(format, args...);
}

template <typename... Types> void warn(std::string format, Types... args)
{
    if (file_logger) {
        file_logger->warn(format, args...);
        if (only_file)
            return;
    }

    spdlog::warn(format, args...);
}

template <typename... Types> void error(std::string format, Types... args)
{
    if (file_logger) {
        file_logger->error(format, args...);
        if (only_file)
            return;
    }

    spdlog::error(format, args...);
}

template <typename... Types> void critical(std::string format, Types... args)
{
    if (file_logger) {
        file_logger->critical(format, args...);
        if (only_file)
            return;
    }

    spdlog::critical(format, args...);
}

inline void init_logger()
{
    spdlog::set_pattern("[%Y-%m-%d %T.%e %z] [%^%l%$] %v");
}

inline void disable_cout()
{
    spdlog::set_level(spdlog::level::off);
}

inline void rotating_logger(const std::string& filename,
                            float max_mb = 5,
                            int max_files = 3,
                            bool only_file_ = false)
{
    std::string path = path_to_abs(filename);
    float max_size = max_mb * 1048576;
    file_logger = spdlog::rotating_logger_mt(
        "rotating_logger", path, max_size, max_files);
    file_logger->flush_on(spdlog::level::info);
    only_file = only_file_;
    if (only_file) {
        spdlog::set_default_logger(file_logger);
    } else {
        // TODO: reset default logger
    }
}
} // namespace yess::log
