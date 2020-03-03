#pragma once

#include "Poco/JSON/Object.h"
#include "Response.hpp"
#include <string>

namespace Leves
{
class ActionHandler
{
  public:
    Response handle(Poco::JSON::Object::Ptr action);
};
} // namespace Leves
