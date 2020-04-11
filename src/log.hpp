#pragma once

#include "../utils/files.hpp"
#include "fmt/format.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/spdlog.h"
#include <string>

namespace yess::log
{
template <typename... Types> void info(std::string format, Types... args)
{
    spdlog::info(format, args...);
}
template <typename... Types> void warn(std::string format, Types... args)
{
    spdlog::warn(format, args...);
}
template <typename... Types> void error(std::string format, Types... args)
{
    spdlog::error(format, args...);
}
template <typename... Types> void critical(std::string format, Types... args)
{
    spdlog::critical(format, args...);
}
static void
setFileLogger(const std::string &filename, float max_mb = 5, int max_files = 3)
{
    std::string path = path_to_abs(filename);
    float max_size = max_mb * 1048576;
    auto rotating_logger = spdlog::rotating_logger_mt(
        "rotating_logger", path, max_size, max_files);
    spdlog::set_default_logger(rotating_logger);
}
} // namespace yess::log
