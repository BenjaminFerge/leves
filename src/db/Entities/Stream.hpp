#pragma once

#include <string>

#include "Poco/JSON/Object.h"

namespace yess::db
{
struct Stream {
    int id;
    std::string type;
    int version = 0;

    Poco::JSON::Object toObject();
};
} // namespace yess::db
