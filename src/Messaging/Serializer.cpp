#include "Serializer.hpp"
#include "../Persistance/Entities/Event.hpp"
#include "../Persistance/Entities/Stream.hpp"
#include <string>
#include <vector>

using namespace Leves::Persistance;

namespace Leves::Messaging
{
std::string serialize(const std::vector<Entities::Event> &events)
{
    // TODO
    return "Not implemented";
}

std::string serialize(const std::vector<Entities::Stream> &streams)
{
    // TODO
    return "Not implemented";
}

std::string serialize(const Entities::Event &event)
{
    // TODO
    return "Not implemented";
}

std::string serialize(const Entities::Stream &stream)
{
    // TODO
    return "Not implemented";
}
} // namespace Leves::Messaging
