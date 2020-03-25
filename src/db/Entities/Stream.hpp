#pragma once

#include "Poco/JSON/Object.h"
#include <cstdint>
#include <string>

namespace leves::db
{
struct Stream {
    int id;
    std::string type;
    int version = 0;

    Poco::JSON::Object toObject();
};
} // namespace leves::db
