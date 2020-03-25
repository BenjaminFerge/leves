#pragma once

#include <cstdint>
#include <string>

#include "Poco/JSON/Object.h"

namespace leves::db
{
struct Stream {
    int id;
    std::string type;
    int version = 0;

    Poco::JSON::Object toObject();
};
} // namespace leves::db
