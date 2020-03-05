#pragma once

#include <cstdint>
#include <string>

namespace Leves::Persistance::Entities
{
struct Stream {
    int id;
    std::string type;
    int version = 0;
};
} // namespace Leves::Persistance::Entities
