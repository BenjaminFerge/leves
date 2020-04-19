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
    } else if (action == action_to_str(Action::None)) {
        return Action::None;
    }
    throw std::runtime_error("Invalid action: " + action);
}

void Action_handler::save_stream(const yess::db::Stream &stream) const
{
    stream_repo_->create(stream);
}

Action_handler::Action_handler(std::string conn_str)
{
    auto stream_repo = db::Sqlite_stream_repo("SQLite", conn_str);
    // Repository initialization in the Server ctor.
    stream_repo_ =
        std::make_unique<db::Sqlite_stream_repo>(std::move(stream_repo));
}

Action_handler::~Action_handler() {}
msg::Response Action_handler::handle(const json &obj)
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
        for (auto &stream : streams) {
            arr.push_back(stream.toJSON());
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

        // Check version
        std::optional<db::Event> lastEventOpt =
            stream_repo_->getLastEvent(streamId);
        int expectedVer;
        if (lastEventOpt.has_value()) {
            auto lastEvent = lastEventOpt.value();
            expectedVer = lastEvent.version + 1;
        } else {
            expectedVer = 1;
        }
        if (expectedVer != version) {
            std::string err = "Event version mismatch: expected v";
            err += std::to_string(expectedVer);
            err += ", got v";
            err += std::to_string(version);
            log::error(err);
            jsonObj["status"] = "Error";
            jsonObj["message"] = err;
            status = msg::ResponseStatus::Error;
            break;
        }

        yess::db::Event event = {0, streamId, type, payload, version};
        try {
            stream_repo_->attachEvent(event);
            jsonObj["status"] = "OK";
            status = msg::ResponseStatus::OK;
        } catch (const std::exception &ex) {
            std::string err = ex.what();
            err = "DB ERROR: " + err;
            log::error(err);
            jsonObj["status"] = "Error";
            jsonObj["message"] = err;
            status = msg::ResponseStatus::Error;
        }
        break;
    }
    case Action::GetEventsByStreamId: {
        int streamId = obj["id"];
        try {
            auto events = stream_repo_->getEvents(streamId);
            jsonObj["status"] = "OK";
            json arr = json::array();
            for (auto &e : events) {
                arr.push_back(e.toJSON());
            }
            jsonObj["data"] = arr;
            status = msg::ResponseStatus::OK;
        } catch (const std::exception &ex) {
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
            for (auto &e : events) {
                arr.push_back(e.toJSON());
            }
            jsonObj["data"] = arr;
            status = msg::ResponseStatus::OK;
        } catch (const std::exception &ex) {
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

Action_handler::Action_handler(Action_handler &&handler)
    : stream_repo_(std::move(handler.stream_repo_))
{
}

std::vector<db::Stream> Action_handler::get_all_streams()
{
    return stream_repo_->all();
}

// TODO: implement
/*
std::vector<db::Stream> Action_handler::get_streams_by_type(std::string type)
{
    return stream_repo_->get(type);
}
*/

db::Stream Action_handler::get_stream(int id) { return db::Stream(); }

void Action_handler::push_event(int stream_id, db::Event event) {}

std::vector<db::Event> Action_handler::get_events_by_stream_id(int stream_id)
{
    return stream_repo_->getEvents(stream_id);
}

std::vector<db::Event>
Action_handler::get_events_by_stream_type(std::string type)
{
    return stream_repo_->getEvents(type);
}
} // namespace yess
