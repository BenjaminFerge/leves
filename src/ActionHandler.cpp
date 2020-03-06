#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "ActionHandler.hpp"
#include "Persistance/Repositories/../Entities/Stream.hpp"
#include "Persistance/Repositories/StreamRepository.hpp"
#include "Poco/Exception.h"
#include "Poco/JSON/Object.h"
#include "Response.hpp"
#include "Server.hpp"

namespace Leves
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

void ActionHandler::saveStream(
    const Leves::Persistance::Entities::Stream &stream)
{
    m_streamRepository->create(stream);
}

ActionHandler::ActionHandler()
{
    std::cout << "Action handler created" << std::endl;
    Application &app = Server::instance();
    std::string connectorKey = (std::string)app.config().getString(
        "EventStore.ConnectorKey", "SQLite");
    std::string connectionString = (std::string)app.config().getString(
        "EventStore.ConnectionString", "leves.db");

    auto streamRepository = Leves::Persistance::Repositories::StreamRepository(
        connectorKey, connectionString);
    m_streamRepository =
        std::make_unique<Leves::Persistance::Repositories::StreamRepository>(
            streamRepository);
}

ActionHandler::~ActionHandler() {}
Response ActionHandler::handle(Poco::JSON::Object::Ptr object)
{
    std::string actionStr = object->getValue<std::string>("action");
    std::string msg;
    Poco::JSON::Object json;
    ResponseStatus status;
    Action action = actionFromString(actionStr);
    std::cout << "Command: " << actionStr << std::endl;
    switch (action) {
    case Action::CreateStream: {
        std::string type = object->getValue<std::string>("type");
        Leves::Persistance::Entities::Stream stream = {0, type, 0};
        saveStream(stream);
        json.set("status", "OK");
        status = ResponseStatus::OK;
        break;
    }
    case Action::GetAllStreams: {
        json.set("status", "OK");
        json.set("action", "getallstreams");
        std::cout << "Querying database..." << std::endl;
        auto streams = m_streamRepository->all();
        std::cout << "OK, length:" << streams.size() << std::endl;
        for (const auto &stream : streams) {
            std::cout << "stream: " << stream.type << std::endl;
        }
        status = ResponseStatus::OK;
        json.set("data", streams);
        break;
    }
    case Action::GetStreamsByType:
        status = ResponseStatus::Error;
        json.set("status", "Error");
        json.set("message", "Action 'GetStreamsByType' is not implemented");
        break;
    case Action::GetStream:
        status = ResponseStatus::Error;
        json.set("status", "Error");
        json.set("message", "Action 'GetStream' is not implemented");
        break;
    case Action::PushEvent: {
        // TODO: Validation
        std::string type = object->getValue<std::string>("type");
        int streamId = object->getValue<int>("streamId");
        std::string payload = object->getValue<std::string>("payload");
        int version = object->getValue<int>("version");
        Leves::Persistance::Entities::Event event = {
            0, streamId, type, payload, version};
        try {
            m_streamRepository->attachEvent(event);
            json.set("status", "OK");
            status = ResponseStatus::OK;
        } catch (const Poco::Exception &ex) {
            std::cerr << "DB ERROR: " << ex.what() << std::endl;
            json.set("status", "Error");
            json.set("message", ex.what());
            status = ResponseStatus::Error;
        }
        break;
    }
    case Action::GetEventsByStreamId:
        status = ResponseStatus::Error;
        json.set("status", "Error");
        json.set("message", "Action 'GetEventsByStreamId' is not implemented");
        break;
    case Action::GetEventsByStreamType:
        status = ResponseStatus::Error;
        json.set("status", "Error");
        json.set("message",
                 "Action 'GetEventsByStreamType' is not implemented");
        break;
    case Action::None:
        status = ResponseStatus::Error;
        json.set("status", "Error");
        json.set("message", "Action 'None' is not implemented");
        break;
    }
    std::ostringstream oss;
    json.stringify(oss);
    msg = oss.str();
    return Response(status, msg);
}
} // namespace Leves
