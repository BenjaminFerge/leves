#include "entity.hpp"
#include <iterator>
#include <nlohmann/json.hpp>

using json_t = nlohmann::json;

namespace yess::db
{
std::ostream& operator<<(std::ostream& os, const Entity& entity)
{
    return entity.display(os);
}
} // namespace yess::db
