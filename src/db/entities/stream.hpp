#pragma once

#include <string>

#include "entity.hpp"
#include "nlohmann/json_fwd.hpp"

namespace yess::db
{
struct Stream : public Entity {
    Stream(int id, std::string type, int version);
    int id;
    std::string type;
    int version;

    [[nodiscard]] nlohmann::json json() const override;
    std::ostream & display(std::ostream &os) const override;
    friend std::ostream& operator<<(std::ostream& os,
                                    const std::vector<Stream>& streams);
};
} // namespace yess::db
