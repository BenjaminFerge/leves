#include <iostream>
#include <string>

#include "action_handler.hpp"
#include "db/repositories/sqlite_stream_repo.hpp"
#include "grpc_service.hpp"
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

void run_server()
{
    std::string server_address("0.0.0.0:2929");
    Grpc_service service("yess.db");

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    grpc::ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

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
    run_server();
    return 0;
}
