#pragma once

#include <string>

#include "nlohmann/json_fwd.hpp"

namespace yess::db
{
struct Event {
    int id;
    int streamId;
    std::string type;
    std::string payload; // JSON
    int version = 0;

    nlohmann::json toJSON();
};
} // namespace yess::db
