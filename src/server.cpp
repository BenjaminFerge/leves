#include <iostream>
#include <string>

#include "action_handler.hpp"
#include "db/repositories/sqlite_stream_repo.hpp"
#include "log.hpp"
#include "server.hpp"
#include "version.h"

using namespace yess;
using namespace db;

Server::Server()
    : requested_info_(Cli_info_opts::none), is_config_loaded_(false)
{
}

Server::~Server() {}

void Server::initDB()
{
    /*
std::string connectorKey =
    (std::string)config().getString("EventStore.ConnectorKey", "SQLite");
std::string connectionString = (std::string)config().getString(
    "EventStore.ConnectionString", "yess.db");

    if (!conn_str_.empty()) {
        connectionString = conn_str_;
    }
*/
    auto streamRepository = Sqlite_stream_repo("SQLite", conn_str_);
    streamRepository.initDB();
}

void Server::handle_option(const std::string &name, const std::string &value)
{
    if (name == "help")
        requested_info_ = Cli_info_opts::help;
    else if (name == "version")
        requested_info_ = Cli_info_opts::version;
    else if (name == "conn-str")
        conn_str_ = value;
    else if (name == "log-path")
        log_path_ = value;

    if (!log_path_.empty()) {
        log::info("Logging path is set to: {}", log_path_);
        log::setFileLogger(log_path_);
    }
}

void Server::display_help() {}

void Server::display_version()
{
    std::cout << YESS_NAME << " v" << YESS_VER << std::endl;
}

std::string Server::conn_str() { return conn_str_; }

int Server::run()
{
    // auto handler = new Action_handler(conn_str_);
    // handler_ = std::make_unique<Action_handler>(std::move(handler));
    handler_ = std::make_unique<Action_handler>(conn_str_);

    switch (requested_info_) {
    case Cli_info_opts::help:
        display_help();
        return 0;
        break;
    case Cli_info_opts::version:
        display_version();
        return 0;
        break;
    case Cli_info_opts::none:
        break;
    }

    log::info("Starting yess on port {}...", port_);
    return 0;
}
