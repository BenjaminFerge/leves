#pragma once

#include <memory>
#include <string>

#include "Persistance/Repositories/StreamRepository.hpp"
#include "Poco/JSON/Object.h"
#include "Response.hpp"

namespace Leves
{
namespace Persistance
{
namespace Entities
{
struct Stream;
} // namespace Entities
} // namespace Persistance

enum Action {
    None,
    CreateStream,
    PushEvent,
    GetAllStreams,
    GetStreamsByType,
    GetStream
};
std::string actionToString(Action action);
Action actionFromString(std::string action);

class ActionHandler
{
  public:
    ActionHandler();
    ~ActionHandler();
    Response handle(Poco::JSON::Object::Ptr action);
    void saveStream(Leves::Persistance::Entities::Stream stream);

  private:
    Persistance::Repositories::StreamRepository m_streamRepository;
};
} // namespace Leves
