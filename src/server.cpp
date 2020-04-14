#include <iostream>
#include <stdexcept>
#include <string>

#include "argparse.hpp"
#include "db/repositories/sqlite_stream_repo.hpp"
#include "grpc_service.hpp"
#include "log.hpp"
#include "server.hpp"
#include "version.h"

using namespace yess;
using namespace db;

Server::Server(int argc, char **argv)
{
    log::init_logger();
    argparser_ = argparse::ArgumentParser("yess");
    parse_args(argc, argv);
}

void Server::parse_args(int argc, char **argv)
{
    argparser_.add_argument("-v", "--version")
        .help("print version number")
        .default_value(false)
        .implicit_value(true);

    argparser_.add_argument("-c", "--conn-str")
        .default_value(conn_str_)
        .help("connection string");

    argparser_.add_argument("-p", "--log-path")
        .help("file log path")
        .default_value(std::string(""));

    argparser_.add_argument("--file-only")
        .help("log into file only")
        .default_value(false)
        .implicit_value(true);

    argparser_.add_argument("-d", "--daemon")
        .help("run in background")
        .default_value(false)
        .implicit_value(true);

    try {
        argparser_.parse_args(argc, argv);
    } catch (const std::runtime_error &err) {
        std::cout << err.what() << std::endl;
        std::cout << argparser_;
        exit(0);
    }

    if (argparser_["--version"] == true) {
        display_version();
        exit(0);
    }

    auto log_path = argparser_.get<std::string>("--log-path");
    bool file_only = argparser_.get<bool>("--file-only");
    if (!log_path.empty()) {
        log_path_ = log_path;
        log::info(
            "Logging path is set to: {} (file only: {})", log_path_, file_only);
        log::rotating_logger(log_path_, 5, 3, file_only);
    } else if (file_only) {
        std::cerr << "Please specify --log-path option when --file-only is set"
                  << std::endl;
        exit(1);
    }

    auto conn_str = argparser_.get<std::string>("--conn-str");
    if (!conn_str.empty()) {
        conn_str_ = conn_str;
        log::info("Connection string is set to: {}", conn_str_);
    }
}

Server::~Server() {}

void Server::initDB()
{
    auto streamRepository = Sqlite_stream_repo("SQLite", conn_str_);
    streamRepository.initDB();
}

void Server::display_version()
{
    std::cout << YESS_NAME << " v" << YESS_VER << std::endl;
}

std::string Server::conn_str() { return conn_str_; }

int Server::run()
{
    std::string server_address("0.0.0.0:" + std::to_string(port_));
    Grpc_service service(conn_str_);

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
    log::info("yess listening on {}...", server_address);

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
    return 0;
}
