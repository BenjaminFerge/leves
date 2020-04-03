#include <sstream>
#include <string>
#include <vector>

#include "../db/Entities/Event.hpp"
#include "../db/Entities/Stream.hpp"
#include "Serializer.hpp"
#include "nlohmann/json.hpp"

using namespace yess::db;
using json = nlohmann::json;

namespace yess::msg
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
    json obj = {{"id", event.id},
                {"streamId", event.streamId},
                {"payload", json::parse(event.payload)},
                {"type", event.type},
                {"version", event.version}};
    return obj.dump();
}

std::string serialize(const Stream &stream)
{
    json obj = {
        {"id", stream.id}, {"type", stream.type}, {"version", stream.version}};
    return obj.dump();
}
} // namespace yess::msg
