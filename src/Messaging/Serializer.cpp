#include "Serializer.hpp"
#include "../Persistance/Entities/Event.hpp"
#include "../Persistance/Entities/Stream.hpp"
#include "Poco/Dynamic/Struct.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Stringifier.h"
#include <sstream>
#include <string>
#include <vector>

using namespace Leves::Persistance;
using namespace Poco::JSON;

namespace Leves::Messaging
{
std::string serialize(const std::vector<Entities::Event> &events)
{
    std::string result = "[";
    std::string delim = "";
    for (const auto &e : events) {
        result += delim;
        result += serialize(e);
        delim = ",";
    }
    result += "]";
    return result;
}

std::string serialize(const std::vector<Entities::Stream> &streams)
{
    std::string result = "[";
    std::string delim = "";
    for (const auto &s : streams) {
        result += delim;
        result += serialize(s);
        delim = ",";
    }
    result += "]";
    return result;
}

std::string serialize(const Entities::Event &event)
{
    Object obj;
    obj.set("id", event.id);
    obj.set("streamId", event.streamId);
    Parser parser;
    Poco::Dynamic::Var payloadVar = parser.parse(event.payload);
    obj.set("payload", payloadVar);
    obj.set("type", event.type);
    obj.set("version", event.version);
    std::string serialized;
    std::ostringstream oss;
    obj.stringify(oss);
    serialized = oss.str();
    return serialized;
}

std::string serialize(const Entities::Stream &stream)
{
    Object obj;
    obj.set("id", stream.id);
    obj.set("type", stream.type);
    obj.set("version", stream.version);
    std::string serialized;
    std::ostringstream oss;
    obj.stringify(oss);
    serialized = oss.str();
    return serialized;
}
} // namespace Leves::Messaging
