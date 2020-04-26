#include <string>

#include "stream.hpp"

#include <nlohmann/json.hpp>
#include <utility>

using json_t = nlohmann::json;

namespace yess::db
{
json_t Stream::json() const
{
    json_t obj;
    obj["id"] = id;
    obj["type"] = type;
    obj["version"] = version;
    return obj;
}
std::ostream& Stream::display(std::ostream& os) const
{
    return os << json();
}
Stream::Stream(int id, std::string type, int version)
    : id(id), type(std::move(type)), version(version)
{
}
std::ostream& operator<<(std::ostream& out, const std::vector<Stream>& streams)
{
    json_t res = json_t::array();
    for (const auto& stream : streams) {
        res.push_back(stream.json());
    }
    out << res.dump(2);
    return out;
}
} // namespace yess::db
