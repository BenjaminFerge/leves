#pragma once

#include <memory>
#include <string>

#include "msg/response.hpp"
#include "nlohmann/json_fwd.hpp"

#include "db/entities/stream.hpp"
#include "db/repositories/projection_repo.hpp"
#include "db/repositories/stream_repository.hpp"
#include "ext/duk_context.hpp"
#include "yess.pb.h"

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
    CreateProjection,
};
std::string action_to_str(Action action);
Action action_from_str(std::string action);

class Action_handler
{
  public:
    Action_handler(std::string conn_str);
    Action_handler(Action_handler&& handler);
    ~Action_handler();
    msg::Response handle(const json& obj);
    void save_stream(const db::Stream& stream) const;

    void create_stream(std::string type) const;
    void delete_stream(int id) const;
    std::vector<db::Stream> get_all_streams() const;
    std::vector<db::Stream> get_streams_by_type(std::string type) const;
    std::optional<db::Stream> get_stream(int id) const;
    void push_event(int stream_id, db::Event event) const;
    std::vector<db::Event> get_events_by_stream_id(int stream_id) const;
    std::vector<db::Event> get_events_by_stream_type(std::string type) const;
    void create_projection(std::string data) const;
    void create_projection(std::string data, std::string type) const;
    std::vector<db::Projection> get_all_projections() const;
    std::vector<db::Projection> get_projections_by_type(std::string type) const;
    void delete_projection(int id) const;
    nlohmann::json
    play_projection(int projection_id,
                    const nlohmann::json& init,
                    const std::string& type,
                    const std::string& fn_name = "projection") const;
    nlohmann::json
    play_projection(int projection_id,
                    const nlohmann::json& init,
                    int stream_id,
                    const std::string& fn_name = "projection") const;

    Action_handler& operator=(Action_handler&& a)
    {
        stream_repo_ = std::move(a.stream_repo_);
        return *this;
    }

  private:
    std::unique_ptr<db::Stream_repository> stream_repo_;
    std::unique_ptr<db::Projection_repo> proj_repo_;
    std::unique_ptr<ext::Duk_context> ctx_;
};
} // namespace yess
