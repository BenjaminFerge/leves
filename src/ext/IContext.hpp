#pragma once

#include "../db/Entities/Event.hpp"
#include "Poco/Dynamic/Var.h"
#include "Poco/JSON/Object.h"
#include <string>

namespace leves::ext
{
class IContext
{
  public:
    virtual Poco::JSON::Object::Ptr
    callProjection(const std::string &fnName,
                   const std::vector<db::Event> &events,
                   Poco::Dynamic::Var initState) = 0;
};
} // namespace leves::ext
