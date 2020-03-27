#pragma once

#include <string>

#include "Poco/JSON/Object.h"

namespace yess::db
{
struct Event {
    int id;
    int streamId;
    std::string type;
    std::string payload; // JSON
    int version = 0;

    Poco::JSON::Object toObject();
};
} // namespace yess::db
