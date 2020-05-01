#pragma once

#include <optional>
#include <vector>

#include "../entities/event.hpp"
#include "../entities/stream.hpp"
#include "sqlite_repository.hpp"

namespace yess::db
{
class Stream_repository
{
  public:
    Stream_repository();
    virtual ~Stream_repository();
    virtual std::vector<Stream> all() = 0;
    virtual std::vector<Stream> by_type(std::string type) = 0;
    virtual std::optional<Stream> get(int id) = 0;
    virtual void create(Stream stream) = 0;
    virtual void attachEvent(Event event) = 0;
    virtual std::vector<Event> getEvents(int streamId) = 0;
    virtual std::vector<Event> getEvents(std::string streamType) = 0;
    virtual std::optional<Event> getLastEvent(int streamId) = 0;
};
} // namespace yess::db
