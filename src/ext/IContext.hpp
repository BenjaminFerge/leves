#pragma once

#include <string>

#include "../db/Entities/Event.hpp"

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace yess::ext
{
class IContext
{
  public:
    virtual json callProjection(const std::string &fnName,
                                const std::vector<db::Event> &events,
                                json initState) = 0;
};
} // namespace yess::ext
