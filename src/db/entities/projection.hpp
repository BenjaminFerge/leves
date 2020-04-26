#ifndef YESS_PROJECTION_HPP
#define YESS_PROJECTION_HPP

#include "entity.hpp"
#include <nlohmann/json.hpp>
#include <string>

namespace yess::db
{
struct Projection : public Entity {
    Projection(int id, std::string data, std::string type);
    int id;
    std::string type;
    std::string data;

    [[nodiscard]] nlohmann::json json() const override;
    std::ostream& display(std::ostream& os) const override;
    friend std::ostream& operator<<(std::ostream& os,
                                    const std::vector<Projection>& projections);
};
} // namespace yess::db

#endif // YESS_PROJECTION_HPP
