#include <nlohmann/json.hpp>
#include <string>

#include "Event.hpp"
#include "Poco/JSON/Array.h"

using namespace Poco::JSON;
using json = nlohmann::json;

namespace yess::db
{
json Event::toJSON()
{
    json obj;
    obj["id"] = id;
    obj["streamId"] = streamId;
    obj["payload"] = json::parse(payload);
    obj["type"] = type;
    obj["version"] = version;
    return obj;
}
} // namespace yess::db
