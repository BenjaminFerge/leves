#ifndef YESS_ENTITY_HPP
#define YESS_ENTITY_HPP
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace yess::db
{
struct Entity {
    [[nodiscard]] virtual nlohmann::json json() const = 0;
    virtual std::ostream& display(std::ostream& os) const = 0;
    friend std::ostream& operator<<(std::ostream& os, const Entity& entity);
};
} // namespace yess::db

#endif // YESS_ENTITY_HPP
