#pragma once

#include "../Persistance/Entities/Event.hpp"
#include "Poco/Dynamic/Var.h"
#include "Poco/JSON/Object.h"
#include <string>
namespace Leves::ext
{
class IContext
{
  public:
    virtual Poco::JSON::Object::Ptr
    callProjection(const std::string &fnName,
                   const std::vector<Persistance::Entities::Event> &events,
                   Poco::Dynamic::Var initState) = 0;
};
} // namespace Leves::ext
