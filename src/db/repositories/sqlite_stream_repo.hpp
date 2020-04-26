#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "../entities/event.hpp"
#include "../entities/stream.hpp"
#include "SQLiteCpp/Database.h"
#include "stream_repository.hpp"

namespace yess::db
{
class Sqlite_stream_repo : public Stream_repository, public Sqlite_repository
{
  public:
    explicit Sqlite_stream_repo(std::shared_ptr<SQLite::Database> db);
    // ~SqliteStreamRepo();
    std::vector<Stream> all() override;
    std::vector<Stream> by_type(std::string type) override;
    Stream get(int id) override;
    void create(Stream stream) override;
    void attachEvent(Event event) override;
    std::vector<Event> getEvents(int streamId) override;
    std::vector<Event> getEvents(std::string streamType) override;
    std::optional<Event> getLastEvent(int streamId) override;
};
} // namespace yess::db
