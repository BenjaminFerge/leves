#include "sqlite_repository.hpp"
#include "../../log.hpp"
#include <SQLiteCpp/Transaction.h>
#include <filesystem>

namespace fs = std::filesystem;

namespace yess::db
{
Sqlite_repository::Sqlite_repository(std::string conn_key, std::string conn_str)
    : conn_key_(conn_key), conn_str_(conn_str)
{
}
void Sqlite_repository::init_db()
{
    log::info("Initializing database...");
    bool shouldReturn = false;
    fs::path p(conn_str_);
    if (fs::exists(conn_str_)) {
        log::info("Database file is already exists");
        // TODO: Check schema
        shouldReturn = true;
    } else {
        log::info("Creating database file...");
    }

    auto db = SQLite::Database(conn_str_,
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
        "stream_id INTEGER, type VARCHAR, payload VARCHAR, version INTEGER, "
        "FOREIGN KEY(stream_id) REFERENCES streams(id), "
        "UNIQUE(stream_id, version))";

    SQLite::Statement stmt2(*db_, createEvents);
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
    SQLite::Statement stmt_tr(*db_, sql);
    stmt_tr.exec();

    // Table: projections
    sql = "CREATE TABLE projections ("
          "id INTEGER PRIMARY KEY, "
          "data VARCHAR, "
          "type VARCHAR)";
    SQLite::Statement stmt_proj(*db_, sql);
    stmt_proj.exec();
}
Sqlite_repository::~Sqlite_repository()
{
}
} // namespace yess::db
