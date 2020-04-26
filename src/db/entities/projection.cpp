#include "projection.hpp"
#include <nlohmann/json.hpp>
#include <ostream>
#include <utility>

using json_t = nlohmann::json;

namespace yess::db
{
std::ostream& Projection::display(std::ostream& os) const
{
    return os << json();
}
json_t Projection::json() const
{
    json_t obj;
    obj["id"] = id;
    obj["type"] = type;
    obj["data"] = data;
    return obj;
}
Projection::Projection(int id, std::string data, std::string type)
    : id(id), data(std::move(data)), type(std::move(type))
{
}
std::ostream& operator<<(std::ostream& out,
                         const std::vector<Projection>& projections)
{
    json_t res = json_t::array();
    for (const auto& proj : projections) {
        res.push_back(proj.json());
    }
    out << res.dump(2);
    return out;
}
} // namespace yess::db
