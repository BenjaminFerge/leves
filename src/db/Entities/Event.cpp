#include <string>

#include "Event.hpp"
#include "Poco/Dynamic/Var.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Array.h"

using namespace Poco::JSON;

namespace leves::db
{
Object Event::toObject()
{
    Object obj;
    obj.set("id", id);
    obj.set("streamId", streamId);
    Parser parser;
    Poco::Dynamic::Var payloadVar = parser.parse(payload);
    obj.set("payload", payloadVar);
    obj.set("type", type);
    obj.set("version", version);
    return obj;
}
} // namespace leves::db
