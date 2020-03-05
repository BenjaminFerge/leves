#include <iosfwd>
#include <memory>

#include "../Entities/Stream.hpp"
#include "Poco/Data/AbstractBinder.h"
#include "Poco/Data/Binding.h"
#include "Poco/Data/SQLite/Connector.h"
#include "Poco/Data/Session.h"
#include "Poco/Data/Statement.h"
#include "StreamRepository.hpp"

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
}

StreamRepository::~StreamRepository() {}

// std::vector<Entities::Stream> StreamRepository::all() {}

// Entities::Stream StreamRepository::get(int id) {}

void StreamRepository::create(Entities::Stream stream)
{
    Statement insert(*m_session);
    insert << "INSERT INTO streams(type, version) VALUES(?, ?)",
        use(stream.type), use(stream.version);

    insert.execute();
}

void StreamRepository::initDB()
{
    *m_session << "DROP TABLE IF EXISTS streams", now;
    *m_session << "CREATE TABLE streams (id INTEGER PRIMARY KEY, "
                  "type VARCHAR, version INTEGER)",
        now;
}
} // namespace Leves::Persistance::Repositories
