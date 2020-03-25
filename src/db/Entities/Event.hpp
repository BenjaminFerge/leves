#pragma once

#include "Poco/JSON/Object.h"
#include <cstdint>
#include <string>

namespace leves::db
{
struct Event {
    int id;
    int streamId;
    std::string type;
    std::string payload; // JSON
    int version = 0;

    Poco::JSON::Object toObject();
};
} // namespace leves::db
