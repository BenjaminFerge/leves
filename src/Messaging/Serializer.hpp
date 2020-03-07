#pragma once

#include "../Persistance/Entities/Event.hpp"
#include "../Persistance/Entities/Stream.hpp"

#include <string>
#include <vector>

namespace Leves::Messaging
{
namespace ents = Persistance::Entities;
std::string serialize(const ents::Stream &stream);
std::string serialize(const ents::Event &events);
std::string serialize(const std::vector<ents::Event> &events);
std::string serialize(const std::vector<ents::Stream> &streams);
} // namespace Leves::Messaging
