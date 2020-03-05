#include <iostream>
#include <stdexcept>
#include <string>

#include "ActionHandler.hpp"
#include "Persistance/Repositories/StreamRepository.hpp"
#include "Response.hpp"
#include "Persistance/Repositories/../Entities/Stream.hpp"

namespace Leves
{
std::string actionToString(Action action)
{
    switch (action) {
    case Action::CreateStream:
        return "CreateStream";
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
    } else if (action == actionToString(Action::PushEvent)) {
        return Action::PushEvent;
    } else if (action == actionToString(Action::None)) {
        return Action::None;
    }
    throw std::runtime_error("Invalid action: " + action);
}

void ActionHandler::saveStream(Leves::Persistance::Entities::Stream stream)
{
    m_streamRepository.create(stream);
}

ActionHandler::ActionHandler()
{
    Persistance::Repositories::StreamRepository m_streamRepository;

    m_streamRepository.initDB();
}

ActionHandler::~ActionHandler() {}
Response ActionHandler::handle(Poco::JSON::Object::Ptr object)
{
    std::string actionStr = object->getValue<std::string>("action");
    Action action = actionFromString(actionStr);
    switch (action) {
    case Action::CreateStream: {
        std::string type = object->getValue<std::string>("type");
        Leves::Persistance::Entities::Stream stream = {type, 0};
        std::cout << "Saving stream..." << std::endl;
        saveStream(stream);
        std::cout << "SAVED" << std::endl;
        break;
    }
    case Action::PushEvent:
        break;
    case Action::None:
        break;
    }
    auto response =
        Response(ResponseStatus::OK, "ACTION: <" + actionStr + ">\n");
    return response;
}
} // namespace Leves
