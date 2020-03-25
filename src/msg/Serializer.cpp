#include <sstream>
#include <string>
#include <vector>

#include "Serializer.hpp"
#include "../db/Entities/Event.hpp"
#include "../db/Entities/Stream.hpp"
#include "Poco/Dynamic/Var.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Array.h"

using namespace leves::db;
using namespace Poco::JSON;

namespace leves::msg
{
std::string serialize(const std::vector<Event> &events)
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

std::string serialize(const std::vector<Stream> &streams)
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

std::string serialize(const Event &event)
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

std::string serialize(const Stream &stream)
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
} // namespace leves::msg
