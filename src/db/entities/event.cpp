#include <string>

#include <nlohmann/json.hpp>
#include <utility>

#include "event.hpp"

using json_t = nlohmann::json;

namespace yess::db
{
json_t Event::json() const
{
    json_t obj;
    obj["id"] = id;
    obj["streamId"] = streamId;
    obj["payload"] = json_t::parse(payload);
    obj["type"] = type;
    obj["version"] = version;
    return obj;
}
std::ostream& Event::display(std::ostream& os) const
{
    return os << json();
}
Event::Event(
    int id, int stream_id, std::string type, std::string payload, int version)
    : id(id), streamId(stream_id), type(std::move(type)),
      payload(std::move(payload)), version(version)
{
}
std::ostream& operator<<(std::ostream& out,
                         const std::vector<Event>& events)
{
    json_t res = json_t::array();
    for (const auto& e : events) {
        res.push_back(e.json());
    }
    out << res.dump(2);
    return out;
}
} // namespace yess::db
