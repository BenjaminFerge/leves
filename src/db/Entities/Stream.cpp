#include <string>

#include "Poco/JSON/Array.h"
#include "Poco/JSON/Object.h"
#include "Stream.hpp"

using namespace Poco::JSON;

namespace yess::db
{
Object Stream::toObject()
{
    Object obj;
    obj.set("id", id);
    obj.set("type", type);
    obj.set("version", version);
    return obj;
}
} // namespace yess::db
