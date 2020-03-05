#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>

#include "../Entities/Stream.hpp"
#include "Poco/Data/AbstractBinder.h"
#include "Poco/Data/Binding.h"
#include "Poco/Data/SQLite/Connector.h"
#include "Poco/Data/Session.h"
#include "Poco/Data/Statement.h"
#include "StreamRepository.hpp"
#include "Poco/Data/Extraction.h"
#include "Poco/Data/Range.h"

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace Leves::Persistance::Repositories
{
StreamRepository::StreamRepository()
{
    Poco::Data::SQLite::Connector::registerConnector();

    Session session("SQLite", "leves.db");
    m_session = std::make_unique<Session>(session);
    initDB();
}

StreamRepository::~StreamRepository() {}

std::vector<Entities::Stream> StreamRepository::all()
{
    Statement select(*m_session);
    Entities::Stream stream;
    select << "SELECT id, type, version FROM streams", into(stream.id),
        into(stream.type), into(stream.version),
        range(0, 1); //  iterate over result set one row at a time

    std::vector<Entities::Stream> result;
    while (!select.done()) {
        select.execute();
        result.push_back(stream);
    }
    std::cout << "RETRIEVED SUCCESFULLY" << std::endl;
    return result;
}

// Entities::Stream StreamRepository::get(int id) {}

void StreamRepository::create(Entities::Stream stream)
{
    Statement insert(*m_session);
    insert << "INSERT INTO streams(type, version) VALUES(?, ?)",
        use(stream.type), use(stream.version);

    insert.execute();
    std::cout << "CREATED SUCCESFULLY" << std::endl;
}

void StreamRepository::initDB()
{
    std::cout << "Database initialized" << std::endl;
    *m_session << "DROP TABLE IF EXISTS streams", now;
    *m_session << "CREATE TABLE streams (id INTEGER PRIMARY KEY, "
                  "type VARCHAR, version INTEGER)",
        now;
}
} // namespace Leves::Persistance::Repositories
