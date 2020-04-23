#pragma once

#include <string>
#include <vector>

#include "../db/entities/event.hpp"
#include "../db/entities/stream.hpp"

namespace yess::msg
{
std::string serialize(const db::Stream& stream);
std::string serialize(const db::Event& events);
std::string serialize(const std::vector<db::Event>& events);
std::string serialize(const std::vector<db::Stream>& streams);
} // namespace yess::msg
