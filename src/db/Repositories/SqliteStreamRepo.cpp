#include <bits/exception.h>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>

#include "../../log.hpp"
#include "../../utils/files.hpp"
#include "../Entities/Event.hpp"
#include "../Entities/Stream.hpp"
#include "SQLiteCpp/Database.h"
#include "SQLiteCpp/Statement.h"
#include "SQLiteCpp/Transaction.h"
#include "SqliteStreamRepo.hpp"
#include "StreamRepository.hpp"
#include "SQLiteCpp/Column.h"

namespace fs = std::filesystem;

namespace yess::db
{
SqliteStreamRepo::SqliteStreamRepo(std::string connectorKey,
                                   std::string connectionString)
    : StreamRepository(connectorKey, pathToAbs(connectionString))
{
    initDB();
}

// SqliteStreamRepo::~SqliteStreamRepo() {}

std::vector<Stream> SqliteStreamRepo::all()
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

Stream SqliteStreamRepo::get(int id)
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

void SqliteStreamRepo::create(Stream stream)
{
    std::string sql = "INSERT INTO streams(type, version) VALUES(?, ?)";
    SQLite::Statement stmt(*db_, sql);

    stmt.bind(1, stream.type);
    stmt.bind(2, stream.version);
    stmt.exec();

    log::info("Created '{}' stream successfully", stream.type);
}

void SqliteStreamRepo::attachEvent(Event event)
{
    std::string sql = "INSERT INTO events(streamId, type, payload, version)"
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

std::vector<Event> SqliteStreamRepo::getEvents(int streamId)
{
    std::string sql = "SELECT e.id, e.streamId, e.type, e.payload, e.version "
                      "FROM events AS e "
                      "INNER JOIN streams AS s ON s.id = e.streamId "
                      "WHERE streamId = ?";
    SQLite::Statement stmt(*db_, sql);

    stmt.bind(1, streamId);

    std::vector<Event> result;
    while (stmt.executeStep()) {
        int id = stmt.getColumn(0);
        int streamId = stmt.getColumn(1);
        std::string type = stmt.getColumn(2);
        std::string payload = stmt.getColumn(3);
        int version = stmt.getColumn(4);

        Event e = {id, streamId, type, payload, version};
        result.push_back(e);
    }
    log::info("Retrieved events by streamId '{}' successfully", streamId);
    return result;
}

std::vector<Event> SqliteStreamRepo::getEvents(std::string streamType)
{
    std::string sql = "SELECT e.id, e.streamId, e.type, e.payload, e.version "
                      "FROM events AS e "
                      "INNER JOIN streams AS s ON s.id = e.streamId "
                      "WHERE s.type = ?";
    SQLite::Statement stmt(*db_, sql);

    stmt.bind(1, streamType);

    std::vector<Event> result;
    while (stmt.executeStep()) {
        int id = stmt.getColumn(0);
        int streamId = stmt.getColumn(1);
        std::string type = stmt.getColumn(2);
        std::string payload = stmt.getColumn(3);
        int version = stmt.getColumn(4);

        Event e = {id, streamId, type, payload, version};
        result.push_back(e);
    }
    log::info("Retrieved events by streamType '{}' successfully", streamType);
    return result;
}

std::optional<Event> SqliteStreamRepo::getLastEvent(int streamId)
{
    std::string sql = "SELECT e.id, e.type, e.payload, e.version "
                      "FROM events AS e "
                      "INNER JOIN streams AS s ON s.id = e.streamId "
                      "WHERE s.id = ? "
                      "ORDER BY e.version "
                      "LIMIT 1";

    SQLite::Statement stmt(*db_, sql);

    stmt.bind(1, streamId);
    stmt.exec();

    if (!stmt.hasRow()) {
        log::info("Stream ID '{}' has no events yet", streamId);
        return std::nullopt;
    }
    int id = stmt.getColumn(0);
    std::string type = stmt.getColumn(1);
    std::string payload = stmt.getColumn(2);
    int version = stmt.getColumn(3);

    Event event = {id, streamId, type, payload, version};
    log::info("Retrieved last event by streamId '{}' successfully", streamId);
    return event;
}

void SqliteStreamRepo::initDB()
{
    log::info("Initializing database...");
    bool shouldReturn = false;
    fs::path p(m_connectionString);
    if (fs::exists(m_connectionString)) {
        log::info("Database file is already exists");
        // TODO: Check schema
        shouldReturn = true;
    } else {
        log::info("Creating database file...");
    }

    auto db = SQLite::Database(m_connectionString,
                               SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    db_ = std::make_unique<SQLite::Database>(std::move(db));
    if (shouldReturn)
        return;

    std::string sql = "PRAGMA foreign_keys = ON";
    SQLite::Statement stmt(*db_, sql);
    stmt.exec();

    log::info("Creating tables...");
    SQLite::Transaction transaction(*db_);
    std::string createStreams = "CREATE TABLE streams (id INTEGER PRIMARY KEY, "
                                "type VARCHAR, version INTEGER)";
    SQLite::Statement stmt1(*db_, createStreams);
    stmt1.exec();
    std::string createEvents =
        "CREATE TABLE events (id INTEGER PRIMARY KEY, "
        "streamId INTEGER, type VARCHAR, payload VARCHAR, version INTEGER, "
        "FOREIGN KEY(streamId) REFERENCES streams(id), "
        "UNIQUE(streamId, version))";

    SQLite::Statement stmt2(*db_, createEvents);
    stmt2.exec();
    transaction.commit();
    log::info("Database initialized successfully");
}
} // namespace yess::db
