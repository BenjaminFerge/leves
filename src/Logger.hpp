#include "spdlog/spdlog.h"

#define LOG_INFO(FORMAT, ...)                                                  \
    {                                                                          \
        spdlog::info((FORMAT), ##__VA_ARGS__);                                 \
    }

#define LOG_WARN(FORMAT, ...)                                                  \
    {                                                                          \
        spdlog::warn((FORMAT), ##__VA_ARGS__);                                 \
    }

#define LOG_ERROR(FORMAT, ...)                                                 \
    {                                                                          \
        spdlog::error((FORMAT), ##__VA_ARGS__);                                \
    }

#define LOG_CRITICAL(FORMAT, ...)                                              \
    {                                                                          \
        spdlog::critical((FORMAT), ##__VA_ARGS__);                             \
    }
