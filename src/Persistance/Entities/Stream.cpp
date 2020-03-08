#include "Stream.hpp"
#include "Poco/JSON/Object.h"
#include <cstdint>
#include <string>

using namespace Poco::JSON;

namespace Leves::Persistance::Entities
{
Object Stream::toObject()
{
    Object obj;
    obj.set("id", id);
    obj.set("type", type);
    obj.set("version", version);
    return obj;
}
} // namespace Leves::Persistance::Entities
