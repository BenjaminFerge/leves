#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"

#include "action_handler.hpp"
#include "db/entities/event.hpp"
#include "db/entities/stream.hpp"
#include "db/repositories/sqlite_projection_repo.hpp"
#include "db/repositories/sqlite_stream_repo.hpp"
#include "db/repositories/stream_repository.hpp"
#include "log.hpp"
#include "msg/response.hpp"
#include "yess.pb.h"

using json = nlohmann::json;

namespace yess
{
std::string action_to_str(Action action)
{
    switch (action) {
    case Action::CreateStream:
        return "CreateStream";
    case Action::GetAllStreams:
        return "GetAllStreams";
    case Action::GetStreamsByType:
        return "GetStreamsByType";
    case Action::GetStream:
        return "GetStream";
    case Action::PushEvent:
        return "PushEvent";
    case Action::GetEventsByStreamId:
        return "GetEventsByStreamId";
    case Action::GetEventsByStreamType:
        return "GetEventsByStreamType";
    case Action::CreateProjection:
        return "CreateProjection";
    case Action::None:
        return "None";
    }
}

Action action_from_str(std::string action)
{
    if (action == action_to_str(Action::CreateStream)) {
        return Action::CreateStream;
    } else if (action == action_to_str(Action::GetAllStreams)) {
        return Action::GetAllStreams;
    } else if (action == action_to_str(Action::GetStreamsByType)) {
        return Action::GetStreamsByType;
    } else if (action == action_to_str(Action::GetStream)) {
        return Action::GetStream;
    } else if (action == action_to_str(Action::PushEvent)) {
        return Action::PushEvent;
    } else if (action == action_to_str(Action::GetEventsByStreamId)) {
        return Action::GetEventsByStreamId;
    } else if (action == action_to_str(Action::GetEventsByStreamType)) {
        return Action::GetEventsByStreamType;
    } else if (action == action_to_str(Action::CreateProjection)) {
        return Action::CreateProjection;
    } else if (action == action_to_str(Action::None)) {
        return Action::None;
    }
    throw std::runtime_error("Invalid action: " + action);
}

void Action_handler::save_stream(const yess::db::Stream& stream) const
{
    stream_repo_->create(stream);
}

Action_handler::Action_handler(std::string conn_str)
{
    // Repository initialization in the Server ctor.
    auto db = db::Sqlite_repository::init_db(conn_str);
    proj_repo_ = std::make_unique<db::Sqlite_projection_repo>(db);
    stream_repo_ = std::make_unique<db::Sqlite_stream_repo>(db);
}

Action_handler::~Action_handler()
{
}
msg::Response Action_handler::handle(const json& obj)
{
    std::string actionStr = obj["action"];
    std::string msg;
    json jsonObj;
    msg::ResponseStatus status;
    Action action = action_from_str(actionStr);
    log::info("Handling command '{}'...", actionStr);
    switch (action) {
    case Action::CreateStream: {
        std::string type = obj["type"];
        yess::db::Stream stream = {0, type, 0};
        save_stream(stream);
        jsonObj["status"] = "OK";
        status = msg::ResponseStatus::OK;
        break;
    }
    case Action::GetAllStreams: {
        jsonObj["status"] = "OK";
        auto streams = stream_repo_->all();
        json arr = json::array();
        for (auto& stream : streams) {
            arr.push_back(stream.json());
        }
        status = msg::ResponseStatus::OK;
        jsonObj["data"] = arr;
        break;
    }
    case Action::GetStreamsByType:
        status = msg::ResponseStatus::Error;
        jsonObj["status"] = "Error";
        jsonObj["message"] = "Action 'GetStreamsByType' is not implemented";
        break;
    case Action::GetStream:
        status = msg::ResponseStatus::Error;
        jsonObj["status"] = "Error";
        jsonObj["message"] = "Action 'GetStream' is not implemented";
        break;
    case Action::PushEvent: {
        std::string type = obj["type"];
        int streamId = obj["streamId"];
        std::string payload = obj["payload"];
        int version = obj["version"];

        yess::db::Event event = {0, streamId, type, payload, version};
        push_event(streamId, event);
        jsonObj["status"] = "OK";
        status = msg::ResponseStatus::OK;
        break;
    }
    case Action::GetEventsByStreamId: {
        int streamId = obj["id"];
        try {
            auto events = stream_repo_->getEvents(streamId);
            jsonObj["status"] = "OK";
            json arr = json::array();
            for (auto& e : events) {
                arr.push_back(e.json());
            }
            jsonObj["data"] = arr;
            status = msg::ResponseStatus::OK;
        } catch (const std::exception& ex) {
            std::string err = ex.what();
            err = "DB ERROR: " + err;
            log::error(err);
            jsonObj["status"] = "Error";
            jsonObj["message"] = err;
            status = msg::ResponseStatus::Error;
        }
        break;
    }
    case Action::GetEventsByStreamType: {
        std::string type = obj["type"];
        try {
            auto events = stream_repo_->getEvents(type);
            jsonObj["status"] = "OK";
            json arr = json::array();
            for (auto& e : events) {
                arr.push_back(e.json());
            }
            jsonObj["data"] = arr;
            status = msg::ResponseStatus::OK;
        } catch (const std::exception& ex) {
            std::string err = ex.what();
            err = "DB ERROR: " + err;
            log::error(err);
            jsonObj["status"] = "Error";
            jsonObj["message"] = err;
            status = msg::ResponseStatus::Error;
        }
        break;
    }
    case Action::None:
        std::string err = "Action 'None' is not implemented";
        log::error(err);
        status = msg::ResponseStatus::Error;
        jsonObj["status"] = "Error";
        jsonObj["message"] = err;
        break;
    }
    msg = jsonObj.dump();
    return msg::Response(status, msg);
}

void Action_handler::create_stream(std::string type) const
{
    yess::db::Stream stream = {0, type, 0};
    save_stream(stream);
}

Action_handler::Action_handler(Action_handler&& handler)
    : stream_repo_(std::move(handler.stream_repo_))
{
}

std::vector<db::Stream> Action_handler::get_all_streams() const
{
    return stream_repo_->all();
}

std::vector<db::Stream>
Action_handler::get_streams_by_type(std::string type) const
{
    return stream_repo_->by_type(type);
}

db::Stream Action_handler::get_stream(int id) const
{
    return stream_repo_->get(id);
}

void Action_handler::push_event(int stream_id, db::Event event) const
{
    try {
        stream_repo_->attachEvent(event);
    } catch (const std::exception& ex) {
        std::string err = ex.what();
        err = "DB ERROR: " + err;
        log::error(err);
        throw std::runtime_error(err);
    }
}

std::vector<db::Event>
Action_handler::get_events_by_stream_id(int stream_id) const
{
    return stream_repo_->getEvents(stream_id);
}

std::vector<db::Event>
Action_handler::get_events_by_stream_type(std::string type) const
{
    return stream_repo_->getEvents(type);
}
void Action_handler::create_projection(std::string data) const
{
    db::Projection p = {-1, "", data};
    proj_repo_->create(p);
}
void Action_handler::create_projection(std::string data, std::string type) const
{
    db::Projection p = {-1, type, data};
    proj_repo_->create(p);
}
std::vector<db::Projection> Action_handler::get_all_projections() const
{
    return proj_repo_->all();
}
std::vector<db::Projection>
Action_handler::get_projections_by_type(std::string type) const
{
    return proj_repo_->type(type);
}
void Action_handler::delete_projection(int id) const
{
    proj_repo_->remove(id);
}
} // namespace yess
