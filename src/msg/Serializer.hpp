#pragma once

#include <string>
#include <vector>

#include "../db/Entities/Event.hpp"
#include "../db/Entities/Stream.hpp"

namespace leves {
namespace db {
struct Event;
struct Stream;
}  // namespace db
}  // namespace leves

namespace leves::msg
{
std::string serialize(const db::Stream &stream);
std::string serialize(const db::Event &events);
std::string serialize(const std::vector<db::Event> &events);
std::string serialize(const std::vector<db::Stream> &streams);
} // namespace leves::msg
