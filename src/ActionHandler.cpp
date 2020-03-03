#include "ActionHandler.hpp"
#include "Poco/JSON/Object.h"
#include "Response.hpp"
#include <string>

namespace Leves
{
Response ActionHandler::handle(Poco::JSON::Object::Ptr object)
{
    std::string action = object->getValue<std::string>("action");
    auto response = Response(ResponseStatus::OK, "ACTION: <" + action + ">\n");
    return response;
}
} // namespace Leves
