#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "../Entities/Event.hpp"
#include "../Entities/Stream.hpp"
#include "SQLiteCpp/Database.h"
#include "StreamRepository.hpp"

namespace yess::db
{
class SqliteStreamRepo : public StreamRepository
{
  public:
    SqliteStreamRepo(std::string connectorKey, std::string connectionString);
    // ~SqliteStreamRepo();
    std::vector<Stream> all();
    Stream get(int id);
    void create(Stream stream);
    void initDB();
    void attachEvent(Event event);
    std::vector<Event> getEvents(int streamId);
    std::vector<Event> getEvents(std::string streamType);
    std::optional<Event> getLastEvent(int streamId);

  private:
    std::unique_ptr<SQLite::Database> db_;
};
} // namespace yess::db
