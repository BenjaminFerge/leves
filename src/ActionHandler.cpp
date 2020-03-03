#include "ActionHandler.hpp"
#include "Persistance/Entities/Stream.hpp"
#include "Poco/Data/SQLite/Connector.h"
#include "Poco/Data/Session.h"
#include "Poco/JSON/Object.h"
#include "Response.hpp"
#include <iostream>
#include <stdexcept>
#include <string>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

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

void saveStream(Leves::Persistance::Entities::Stream stream)
{
    // register SQLite connector
    Poco::Data::SQLite::Connector::registerConnector();

    // create a session
    Session session("SQLite", "leves.db");

    // drop sample table, if it exists
    session << "DROP TABLE IF EXISTS streams", now;

    // (re)create table
    session << "CREATE TABLE streams (id INTEGER PRIMARY KEY, type "
               "VARCHAR(30), version INTEGER(3))",
        now;

    // insert some rows
    Statement insert(session);
    insert << "INSERT INTO streams(type, version) VALUES(?, ?)",
        use(stream.type), use(stream.version);

    insert.execute();
}

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
