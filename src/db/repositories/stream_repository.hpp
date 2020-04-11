#pragma once

#include <optional>
#include <string>
#include <vector>

#include "../entities/event.hpp"
#include "../entities/stream.hpp"

namespace yess::db
{
class Stream_repository
{
  public:
    Stream_repository(std::string connectorKey, std::string connectionString);
    virtual ~Stream_repository();
    virtual std::vector<Stream> all() = 0;
    virtual Stream get(int id) = 0;
    virtual void create(Stream stream) = 0;
    virtual void initDB() = 0;
    virtual void attachEvent(Event event) = 0;
    virtual std::vector<Event> getEvents(int streamId) = 0;
    virtual std::vector<Event> getEvents(std::string streamType) = 0;
    virtual std::optional<Event> getLastEvent(int streamId) = 0;

  protected:
    std::string m_connectorKey;
    std::string m_connectionString;
};
} // namespace yess::db
