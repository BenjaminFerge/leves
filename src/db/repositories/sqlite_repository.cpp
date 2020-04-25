#include "sqlite_repository.hpp"
#include "../../log.hpp"
#include <SQLiteCpp/Transaction.h>
#include <filesystem>

namespace fs = std::filesystem;

namespace yess::db
{
Sqlite_repository::Sqlite_repository(std::shared_ptr<SQLite::Database> db)
    : db_(db)
{
}
std::shared_ptr<SQLite::Database> Sqlite_repository::init_db(std::string conn_str)
{
    log::info("Initializing database...");
    bool shouldReturn = false;
    fs::path p(conn_str);
    if (fs::exists(conn_str)) {
        log::info("Database file is already exists");
        // TODO: Check schema
        shouldReturn = true;
    } else {
        log::info("Creating database file...");
    }

    auto db = std::make_shared<SQLite::Database>(conn_str,
                               SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    if (shouldReturn)
        return db;

    std::string sql = "PRAGMA foreign_keys = ON";
    SQLite::Statement stmt(*db, sql);
    stmt.exec();

    log::info("Creating tables...");
    SQLite::Transaction transaction(*db);
    std::string createStreams = "CREATE TABLE streams (id INTEGER PRIMARY KEY, "
                                "type VARCHAR, version INTEGER)";
    SQLite::Statement stmt1(*db, createStreams);
    stmt1.exec();
    std::string createEvents =
        "CREATE TABLE events (id INTEGER PRIMARY KEY, "
        "stream_id INTEGER, type VARCHAR, payload VARCHAR, version INTEGER, "
        "FOREIGN KEY(stream_id) REFERENCES streams(id), "
        "UNIQUE(stream_id, version))";

    SQLite::Statement stmt2(*db, createEvents);
    stmt2.exec();
    transaction.commit();
    log::info("Database initialized successfully");

    sql = "CREATE TRIGGER ver_valid BEFORE INSERT ON events "
          "BEGIN "
          "SELECT CASE "
          "WHEN ((SELECT count(*) FROM events "
          "WHERE events.stream_id = NEW.stream_id) = 0 "
          "AND (NEW.version != 1)) "
          "THEN RAISE(ABORT, 'The stream should starts with event version 1.') "
          "WHEN ((SELECT events.version FROM events "
          "WHERE events.stream_id = NEW.stream_id "
          "ORDER BY events.version DESC "
          "LIMIT 1) != (NEW.version - 1)) "
          // TODO: show the expectations
          "THEN RAISE(ABORT, 'Event version mismatch.') "
          "END; "
          "END;";
    SQLite::Statement stmt_tr(*db, sql);
    stmt_tr.exec();

    // Table: projections
    sql = "CREATE TABLE projections ("
          "id INTEGER PRIMARY KEY, "
          "data VARCHAR, "
          "type VARCHAR)";
    SQLite::Statement stmt_proj(*db, sql);
    stmt_proj.exec();
    return db;
}
Sqlite_repository::~Sqlite_repository()
{
}
} // namespace yess::db
