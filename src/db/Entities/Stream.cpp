#include <string>

#include "Stream.hpp"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Array.h"

using namespace Poco::JSON;

namespace leves::db
{
Object Stream::toObject()
{
    Object obj;
    obj.set("id", id);
    obj.set("type", type);
    obj.set("version", version);
    return obj;
}
} // namespace leves::db
