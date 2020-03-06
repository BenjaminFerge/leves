#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "ActionHandler.hpp"
#include "Persistance/Repositories/../Entities/Stream.hpp"
#include "Persistance/Repositories/StreamRepository.hpp"
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
    } else if (action == actionToString(Action::None)) {
        return Action::None;
    }
    throw std::runtime_error("Invalid action: " + action);
}

void ActionHandler::saveStream(
    const Leves::Persistance::Entities::Stream &stream)
{
    m_pStreamRepository->create(std::move(stream));
}

ActionHandler::ActionHandler()
{
    std::cout << "Action handler created" << std::endl;
    // Application &app = Server::instance();
    // Server &server = dynamic_cast<Server &>(app);
    // m_pServer = &server;
    // m_pStreamRepository = server.getStreamRepository();
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
        saveStream(std::move(stream));
        json.set("status", "OK");
        status = ResponseStatus::OK;
        break;
    }
    case Action::GetAllStreams: {
        json.set("status", "OK");
        json.set("action", "getallstreams");
        std::cout << "Querying database..." << std::endl;
        auto streams = m_pStreamRepository->all();
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
    case Action::PushEvent:
        status = ResponseStatus::Error;
        json.set("status", "Error");
        json.set("message", "Action 'PushEvent' is not implemented");
    case Action::None:
        status = ResponseStatus::Error;
        json.set("status", "Error");
        json.set("message", "Action 'None' is not implemented");
    }
    std::ostringstream oss;
    json.stringify(oss);
    msg = oss.str();
    return Response(status, msg);
}
} // namespace Leves
