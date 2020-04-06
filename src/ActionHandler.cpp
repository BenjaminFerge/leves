#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "ActionHandler.hpp"
#include "Poco/Exception.h"
#include "Poco/Util/Application.h"
#include "Poco/Util/LayeredConfiguration.h"
#include "Response.hpp"
#include "Server.hpp"
#include "db/Entities/Stream.hpp"
#include "db/Repositories/../Entities/Event.hpp"
#include "db/Repositories/SqliteStreamRepo.hpp"
#include "db/Repositories/StreamRepository.hpp"
#include "log.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace yess
{
std::string actionToString(Action action)
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

Action actionFromString(std::string action)
{
    if (action == actionToString(Action::CreateStream)) {
        return Action::CreateStream;
    } else if (action == actionToString(Action::GetAllStreams)) {
        return Action::GetAllStreams;
    } else if (action == actionToString(Action::GetStreamsByType)) {
        return Action::GetStreamsByType;
    } else if (action == actionToString(Action::GetStream)) {
        return Action::GetStream;
    } else if (action == actionToString(Action::PushEvent)) {
        return Action::PushEvent;
    } else if (action == actionToString(Action::GetEventsByStreamId)) {
        return Action::GetEventsByStreamId;
    } else if (action == actionToString(Action::GetEventsByStreamType)) {
        return Action::GetEventsByStreamType;
    } else if (action == actionToString(Action::None)) {
        return Action::None;
    }
    throw std::runtime_error("Invalid action: " + action);
}

void ActionHandler::saveStream(const yess::db::Stream &stream)
{
    m_streamRepository->create(stream);
}

ActionHandler::ActionHandler()
{
    Application &app = Server::instance();
    Server *yess = static_cast<Server *>(&app);
    auto connStr = yess->getConnStr();
    // TODO: DRY
    std::string connectorKey = (std::string)app.config().getString(
        "EventStore.ConnectorKey", "SQLite");
    std::string connectionString = (std::string)app.config().getString(
        "EventStore.ConnectionString", "yess.db");
    if (!connStr.empty()) {
        connectionString = connStr;
    }
    auto streamRepository =
        db::SqliteStreamRepo(connectorKey, connectionString);
    // Repository initialization in the Server ctor.
    m_streamRepository =
        std::make_unique<db::SqliteStreamRepo>(std::move(streamRepository));
}

ActionHandler::~ActionHandler() {}
Response ActionHandler::handle(const json &obj)
{
    std::string actionStr = obj["action"];
    std::string msg;
    json jsonObj;
    ResponseStatus status;
    Action action = actionFromString(actionStr);
    log::info("Handling command '{}'...", actionStr);
    switch (action) {
    case Action::CreateStream: {
        std::string type = obj["type"];
        yess::db::Stream stream = {0, type, 0};
        saveStream(stream);
        jsonObj["status"] = "OK";
        status = ResponseStatus::OK;
        break;
    }
    case Action::GetAllStreams: {
        jsonObj["status"] = "OK";
        auto streams = m_streamRepository->all();
        json arr = json::array();
        for (auto &stream : streams) {
            arr.push_back(stream.toJSON());
        }
        status = ResponseStatus::OK;
        jsonObj["data"] = arr;
        break;
    }
    case Action::GetStreamsByType:
        status = ResponseStatus::Error;
        jsonObj["status"] = "Error";
        jsonObj["message"] = "Action 'GetStreamsByType' is not implemented";
        break;
    case Action::GetStream:
        status = ResponseStatus::Error;
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
            m_streamRepository->getLastEvent(streamId);
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
            status = ResponseStatus::Error;
            break;
        }

        yess::db::Event event = {0, streamId, type, payload, version};
        try {
            m_streamRepository->attachEvent(event);
            jsonObj["status"] = "OK";
            status = ResponseStatus::OK;
        } catch (const Poco::Exception &ex) {
            std::string err = ex.displayText();
            err = "DB ERROR: " + err;
            log::error(err);
            jsonObj["status"] = "Error";
            jsonObj["message"] = err;
            status = ResponseStatus::Error;
        }
        break;
    }
    case Action::GetEventsByStreamId: {
        int streamId = obj["id"];
        try {
            auto events = m_streamRepository->getEvents(streamId);
            jsonObj["status"] = "OK";
            json arr = json::array();
            for (auto &e : events) {
                arr.push_back(e.toJSON());
            }
            jsonObj["data"] = arr;
            status = ResponseStatus::OK;
        } catch (const Poco::Exception &ex) {
            std::string err = ex.what();
            err = "DB ERROR: " + err;
            log::error(err);
            jsonObj["status"] = "Error";
            jsonObj["message"] = err;
            status = ResponseStatus::Error;
        }
        break;
    }
    case Action::GetEventsByStreamType: {
        std::string type = obj["type"];
        try {
            auto events = m_streamRepository->getEvents(type);
            jsonObj["status"] = "OK";
            json arr = json::array();
            for (auto &e : events) {
                arr.push_back(e.toJSON());
            }
            jsonObj["data"] = arr;
            status = ResponseStatus::OK;
        } catch (const Poco::Exception &ex) {
            std::string err = ex.what();
            err = "DB ERROR: " + err;
            log::error(err);
            jsonObj["status"] = "Error";
            jsonObj["message"] = err;
            status = ResponseStatus::Error;
        }
        break;
    }
    case Action::None:
        std::string err = "Action 'None' is not implemented";
        log::error(err);
        status = ResponseStatus::Error;
        jsonObj["status"] = "Error";
        jsonObj["message"] = err;
        break;
    }
    msg = jsonObj.dump();
    return Response(status, msg);
}
} // namespace yess
