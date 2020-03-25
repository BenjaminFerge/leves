#pragma once

#include "../db/Entities/Event.hpp"
#include "../db/Entities/Stream.hpp"

#include <string>
#include <vector>

namespace leves::msg
{
std::string serialize(const db::Stream &stream);
std::string serialize(const db::Event &events);
std::string serialize(const std::vector<db::Event> &events);
std::string serialize(const std::vector<db::Stream> &streams);
} // namespace leves::msg
