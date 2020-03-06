#pragma once

#include <memory>
#include <string>

#include "Persistance/Repositories/StreamRepository.hpp"
#include "Poco/JSON/Object.h"
#include "Response.hpp"
#include "Server.hpp"

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
    void saveStream(const Leves::Persistance::Entities::Stream &stream);

  private:
    Server *m_pServer;
    Persistance::Repositories::StreamRepository *m_pStreamRepository;
};
} // namespace Leves
