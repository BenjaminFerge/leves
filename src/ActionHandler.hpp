#pragma once

#include <string>

#include "Poco/JSON/Object.h"
#include "Response.hpp"

namespace Leves
{
class ActionHandler
{
  public:
    Response handle(Poco::JSON::Object::Ptr action);
};
} // namespace Leves
