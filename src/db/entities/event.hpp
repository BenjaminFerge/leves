#pragma once

#include <string>

#include "entity.hpp"
#include "nlohmann/json_fwd.hpp"

namespace yess::db
{
struct Event : public Entity {
    Event(int id,
          int stream_id,
          std::string type,
          std::string payload,
          int version);
    int id;
    int streamId;
    std::string type;
    std::string payload; // JSON
    int version;

    [[nodiscard]] nlohmann::json json() const override;
    std::ostream & display(std::ostream &os) const override;
    friend std::ostream& operator<<(std::ostream& os,
                                    const std::vector<Event>& events);
};
} // namespace yess::db
