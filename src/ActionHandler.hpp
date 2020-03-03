#pragma once

#include <string>

#include "Poco/JSON/Object.h"
#include "Response.hpp"

namespace Leves
{
enum Action { None, CreateStream, PushEvent };
std::string actionToString(Action action);
Action actionFromString(std::string action);

class ActionHandler
{
  public:
    Response handle(Poco::JSON::Object::Ptr action);
};
} // namespace Leves
