#pragma once

#include <cstdint>
#include <string>

namespace Leves::Persistance::Entities
{
struct Stream {
    std::string type;
    int version = 0;
};
} // namespace Leves::Persistance::Entities
