#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../Entities/Stream.hpp"
#include "Poco/Data/AbstractBinder.h"
#include "Poco/Data/Binding.h"
#include "Poco/Data/Extraction.h"
#include "Poco/Data/Range.h"
#include "Poco/Data/SQLite/Connector.h"
#include "Poco/Data/Session.h"
#include "Poco/Data/Statement.h"
#include "StreamRepository.hpp"

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace Leves::Persistance::Repositories
{
StreamRepository::StreamRepository(std::string connectorKey,
                                   std::string connectionString)
    : m_connectorKey(connectorKey), m_connetctionString(connectionString)
{
    Poco::Data::SQLite::Connector::registerConnector();
}

StreamRepository::~StreamRepository() {}

Session StreamRepository::makeSession()
{
    auto session = Session(m_connectorKey, m_connetctionString);
    session << "PRAGMA foreign_keys = ON", now;
    return session;
}

std::vector<Entities::Stream> StreamRepository::all()
{
    Session session = Session(m_connectorKey, m_connetctionString);
    Statement select(session);
    Entities::Stream stream;
    select << "SELECT id, type, version FROM streams", into(stream.id),
        into(stream.type), into(stream.version),
        range(0, 1); //  iterate over result set one row at a time

    std::vector<Entities::Stream> result;
    while (!select.done()) {
        select.execute();
        result.push_back(stream);
    }
    std::cout << "RETRIEVED SUCCESSFULLY" << std::endl;
    return result;
}

// Entities::Stream StreamRepository::get(int id) {}

void StreamRepository::create(Entities::Stream stream)
{
    Session session = makeSession();
    Statement insert(session);
    insert << "INSERT INTO streams(type, version) VALUES(?, ?)",
        use(stream.type), use(stream.version);

    insert.execute();
    std::cout << "CREATED SUCCESSFULLY" << std::endl;
}

void StreamRepository::attachEvent(Entities::Event event)
{
    Session session = makeSession();
    Statement insert(session);
    insert << "INSERT INTO events(streamId, type, payload, version)"
              "VALUES(?, ?, ?, ?)",
        use(event.streamId), use(event.type), use(event.payload),
        use(event.version);

    insert.execute();
    std::cout << "ATTACHED SUCCESSFULLY" << std::endl;
}

std::vector<Entities::Event> StreamRepository::getEvents(int streamId)
{
    Session session = makeSession();
    Statement select(session);
    Entities::Event event;
    select << "SELECT id, streamId, type, payload, version "
              "FROM events e "
              "INNER JOIN streams s on s.id = e.streamId "
              "WHERE streamId = ?",
        use(streamId), into(event.id), into(event.streamId), into(event.type),
        into(event.payload), into(event.version),
        range(0, 1); //  iterate over result set one row at a time

    std::vector<Entities::Event> result;
    while (!select.done()) {
        select.execute();
        result.push_back(event);
    }
    std::cout << "RETRIEVED SUCCESSFULLY" << std::endl;
    return result;
}

std::vector<Entities::Event> StreamRepository::getEvents(std::string streamType)
{
    Session session = makeSession();
    Statement select(session);
    Entities::Event event;
    select << "SELECT id, streamId, type, payload, version "
              "FROM events e "
              "INNER JOIN streams s on s.id = e.streamId "
              "WHERE s.type = ?",
        use(streamType), into(event.id), into(event.streamId), into(event.type),
        into(event.payload), into(event.version),
        range(0, 1); //  iterate over result set one row at a time

    std::vector<Entities::Event> result;
    while (!select.done()) {
        select.execute();
        result.push_back(event);
    }
    std::cout << "RETRIEVED SUCCESSFULLY" << std::endl;
    return result;
}

void StreamRepository::initDB()
{
    Session session = makeSession();
    std::cout << "Database initialized" << std::endl;
    session << "DROP TABLE IF EXISTS streams", now;
    session << "DROP TABLE IF EXISTS events", now;
    session << "CREATE TABLE streams (id INTEGER PRIMARY KEY, "
               "type VARCHAR, version INTEGER)",
        now;
    session
        << "CREATE TABLE events (id INTEGER PRIMARY KEY, "
           "streamId INTEGER, type VARCHAR, payload VARCHAR, version INTEGER, "
           "FOREIGN KEY(streamId) REFERENCES streams(id))",
        now;
}
} // namespace Leves::Persistance::Repositories
