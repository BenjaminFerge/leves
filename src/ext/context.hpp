#pragma once

#include <string>

#include "../db/entities/event.hpp"

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace yess::ext
{
class Context
{
  public:
    virtual json call_projection(const std::string& fn_name,
                                 const std::vector<db::Event>& events,
                                 json init_state) = 0;
};
} // namespace yess::ext
