#pragma once

#include <memory>
#include <string>

#include "msg/response.hpp"
#include "nlohmann/json_fwd.hpp"

#include "db/entities/stream.hpp"
#include "db/repositories/stream_repository.hpp"

using nlohmann::json;

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
std::string action_to_str(Action action);
Action action_from_str(std::string action);

class Action_handler
{
  public:
    Action_handler(std::string conn_str);
    ~Action_handler();
    msg::Response handle(const json &obj);
    void save_stream(const db::Stream &stream);

  private:
    std::unique_ptr<db::Stream_repository> stream_repo_;
};
} // namespace yess
