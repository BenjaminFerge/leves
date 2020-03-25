#pragma once

#include <string>
#include <vector>

#include "../Entities/Event.hpp"
#include "../Entities/Stream.hpp"
#include "Poco/Data/Session.h"

namespace leves::db
{
class StreamRepository
{
  public:
    StreamRepository(std::string connectorKey, std::string connectionString);
    ~StreamRepository();
    std::vector<Stream> all();
    Stream get(int id);
    void create(Stream stream);
    void initDB();
    void attachEvent(Event event);
    std::vector<Event> getEvents(int streamId);
    std::vector<Event> getEvents(std::string streamType);
    Poco::Data::Session makeSession();

  private:
    std::string m_connectorKey;
    std::string m_connetctionString;
};
} // namespace leves::db
