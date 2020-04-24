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
    Sqlite_stream_repo(std::string conn_key, std::string conn_str);
    // ~SqliteStreamRepo();
    std::vector<Stream> all();
    Stream get(int id);
    void create(Stream stream);
    void attachEvent(Event event);
    std::vector<Event> getEvents(int streamId);
    std::vector<Event> getEvents(std::string streamType);
    std::optional<Event> getLastEvent(int streamId);
};
} // namespace yess::db
