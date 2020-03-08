#pragma once

#include "Poco/JSON/Object.h"
#include <cstdint>
#include <string>

namespace Leves::Persistance::Entities
{
struct Stream {
    int id;
    std::string type;
    int version = 0;

    Poco::JSON::Object toObject();
};
} // namespace Leves::Persistance::Entities
