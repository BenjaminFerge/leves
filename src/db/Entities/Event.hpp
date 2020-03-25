#pragma once

#include <cstdint>
#include <string>

#include "Poco/JSON/Object.h"

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
