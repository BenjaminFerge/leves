#pragma once

#include <string>              // for string

#include "Poco/JSON/Object.h"  // for Object

namespace yess::db
{
struct Stream {
    int id;
    std::string type;
    int version = 0;

    Poco::JSON::Object toObject();
};
} // namespace yess::db
