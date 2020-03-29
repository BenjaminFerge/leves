#pragma once

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
} // namespace yess::log
