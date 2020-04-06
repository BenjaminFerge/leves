#include <string>

#include "Stream.hpp"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace yess::db
{
json Stream::toJSON()
{
    json obj;
    obj["id"] = id;
    obj["type"] = type;
    obj["version"] = version;
    return obj;
}
} // namespace yess::db
