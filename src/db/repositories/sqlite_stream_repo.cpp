#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "../../log.hpp"
#include "../entities/event.hpp"
#include "../entities/stream.hpp"
#include "SQLiteCpp/Database.h"
#include "SQLiteCpp/Statement.h"
#include "SQLiteCpp/Transaction.h"
#include "sqlite_stream_repo.hpp"
#include "stream_repository.hpp"

namespace fs = std::filesystem;

namespace yess::db
{
Sqlite_stream_repo::Sqlite_stream_repo(std::string conn_key,
                                       std::string conn_str)
    : Sqlite_repository(conn_key, path_to_abs(conn_str))
{
}

// SqliteStreamRepo::~SqliteStreamRepo() {}

std::vector<Stream> Sqlite_stream_repo::all()
{

    std::string sql = "SELECT id, type, version FROM streams";
    SQLite::Statement stmt(*db_, sql);

    std::vector<Stream> result;
    while (stmt.executeStep()) {
        int id = stmt.getColumn(0);
        std::string type = stmt.getColumn(1);
        int version = stmt.getColumn(2);

        Stream s = {id, type, version};
        result.push_back(s);
    }
    log::info("Retrieved all streams successfully");
    return result;
}

Stream Sqlite_stream_repo::get(int id)
{

    std::string sql = "SELECT type, version "
                      "FROM streams "
                      "WHERE id = ?";

    SQLite::Statement stmt(*db_, sql);

    stmt.bind(1, id);
    stmt.exec();

    std::string type = stmt.getColumn(0);
    int version = stmt.getColumn(1);

    Stream stream = {id, type, version};
    log::info("Retrieved stream by ID '{}' successfully", id);
    return stream;
}

void Sqlite_stream_repo::create(Stream stream)
{
    std::string sql = "INSERT INTO streams(type, version) VALUES(?, ?)";
    SQLite::Statement stmt(*db_, sql);

    stmt.bind(1, stream.type);
    stmt.bind(2, stream.version);
    stmt.exec();

    log::info("Created '{}' stream successfully", stream.type);
}

void Sqlite_stream_repo::attachEvent(Event event)
{
    std::string sql = "INSERT INTO events(stream_id, type, payload, version)"
                      "VALUES(?, ?, ?, ?)";
    SQLite::Statement stmt(*db_, sql);

    stmt.bind(1, event.streamId);
    stmt.bind(2, event.type);
    stmt.bind(3, event.payload);
    stmt.bind(4, event.version);
    stmt.exec();

    log::info("Atteched '{}' (v{}) event into stream ID {}",
              event.type,
              event.version,
              event.streamId);
}

std::vector<Event> Sqlite_stream_repo::getEvents(int stream_id)
{
    std::string sql = "SELECT e.id, e.stream_id, e.type, e.payload, e.version "
                      "FROM events AS e "
                      "INNER JOIN streams AS s ON s.id = e.stream_id "
                      "WHERE stream_id = ?";
    SQLite::Statement stmt(*db_, sql);

    stmt.bind(1, stream_id);

    std::vector<Event> result;
    while (stmt.executeStep()) {
        int id = stmt.getColumn(0);
        int stream_id = stmt.getColumn(1);
        std::string type = stmt.getColumn(2);
        std::string payload = stmt.getColumn(3);
        int version = stmt.getColumn(4);

        Event e = {id, stream_id, type, payload, version};
        result.push_back(e);
    }
    log::info("Retrieved events by stream_id '{}' successfully", stream_id);
    return result;
}

std::vector<Event> Sqlite_stream_repo::getEvents(std::string streamType)
{
    std::string sql = "SELECT e.id, e.stream_id, e.type, e.payload, e.version "
                      "FROM events AS e "
                      "INNER JOIN streams AS s ON s.id = e.stream_id "
                      "WHERE s.type = ?";
    SQLite::Statement stmt(*db_, sql);

    stmt.bind(1, streamType);

    std::vector<Event> result;
    while (stmt.executeStep()) {
        int id = stmt.getColumn(0);
        int stream_id = stmt.getColumn(1);
        std::string type = stmt.getColumn(2);
        std::string payload = stmt.getColumn(3);
        int version = stmt.getColumn(4);

        Event e = {id, stream_id, type, payload, version};
        result.push_back(e);
    }
    log::info("Retrieved events by streamType '{}' successfully", streamType);
    return result;
}

std::optional<Event> Sqlite_stream_repo::getLastEvent(int stream_id)
{
    std::string sql = "SELECT e.id, e.type, e.payload, e.version "
                      "FROM events AS e "
                      "INNER JOIN streams AS s ON s.id = e.stream_id "
                      "WHERE s.id = ? "
                      "ORDER BY e.version DESC "
                      "LIMIT 1";

    SQLite::Statement stmt(*db_, sql);

    stmt.bind(1, stream_id);
    stmt.executeStep();

    if (!stmt.hasRow()) {
        log::info("Stream ID '{}' has no events yet", stream_id);
        return std::nullopt;
    }
    int id = stmt.getColumn(0);
    std::string type = stmt.getColumn(1);
    std::string payload = stmt.getColumn(2);
    int version = stmt.getColumn(3);

    Event event = {id, stream_id, type, payload, version};
    log::info("Retrieved last event by stream_id '{}' successfully", stream_id);
    return event;
}
} // namespace yess::db
