#pragma once

#include <string>

#include "nlohmann/json_fwd.hpp"

namespace yess::db
{
struct Stream {
    int id;
    std::string type;
    int version = 0;

    nlohmann::json toJSON();
};
} // namespace yess::db
