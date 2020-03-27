#pragma once

#include <memory>                                  // for unique_ptr
#include <string>                                  // for string

#include "Poco/JSON/Object.h"                      // for Object, Object::Ptr
#include "Response.hpp"                            // for Response
#include "Server.hpp"                              // for Server
#include "db/Repositories/StreamRepository.hpp"    // for StreamRepository
#include "db/Repositories/../Entities/Stream.hpp"  // for Stream

namespace yess
{
enum Action {
    None,
    CreateStream,
    PushEvent,
    GetAllStreams,
    GetStreamsByType,
    GetStream,
    GetEventsByStreamId,
    GetEventsByStreamType,
};
std::string actionToString(Action action);
Action actionFromString(std::string action);

class ActionHandler
{
  public:
    ActionHandler();
    ~ActionHandler();
    Response handle(Poco::JSON::Object::Ptr action);
    void saveStream(const db::Stream &stream);

  private:
    Server *m_pServer;
    std::unique_ptr<db::StreamRepository> m_streamRepository;
};
} // namespace yess
