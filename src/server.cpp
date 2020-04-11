#include <iostream>
#include <string>

#include "Poco/Foundation.h"
#include "Poco/Net/Net.h"
#include "Poco/Util/Subsystem.h"
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/SocketAcceptor.h>
#include <Poco/Net/SocketReactor.h>
#include <Poco/Thread.h>
#include <Poco/Util/Application.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Util/LayeredConfiguration.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/ServerApplication.h>

#include "db/repositories/sqlite_stream_repo.hpp"
#include "log.hpp"
#include "server.hpp"
#include "service_handler.hpp"
#include "version.h"

using namespace yess;
using namespace db;
using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;

Server::Server()
    : requested_info_(Cli_info_opts::none), is_config_loaded_(false)
{
}

Server::~Server() {}

void Server::initDB()
{
    std::string connectorKey =
        (std::string)config().getString("EventStore.ConnectorKey", "SQLite");
    std::string connectionString = (std::string)config().getString(
        "EventStore.ConnectionString", "yess.db");

    if (!conn_str_.empty()) {
        connectionString = conn_str_;
    }
    auto streamRepository = Sqlite_stream_repo(connectorKey, connectionString);
    streamRepository.initDB();
}

void Server::initialize(Application &self)
{
    // load default configuration files, if present
    is_config_loaded_ = loadConfiguration();
    ServerApplication::initialize(self);
    initDB();
}

void Server::uninitialize() { ServerApplication::uninitialize(); }

void Server::define_options(OptionSet &options)
{
    ServerApplication::defineOptions(options);

    auto help = Option("help",
                       "h",
                       "Display help information on command line arguments")
                    .required(false)
                    .repeatable(false);
    options.addOption(help);
    auto version = Option("version", "v", "Print version information")
                       .required(false)
                       .repeatable(false);
    options.addOption(version);

    auto connStr = Option("conn-str", "c", "Connection string")
                       .required(false)
                       .argument("<string>")
                       .repeatable(false);
    options.addOption(connStr);

    auto logPath = Option("log-path", "l", "Log path")
                       .required(false)
                       .argument("<path>")
                       .repeatable(false);
    options.addOption(logPath);
}

void Server::handle_option(const std::string &name, const std::string &value)
{
    ServerApplication::handleOption(name, value);

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

void Server::display_help()
{
    HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.setHeader(about_str);
    helpFormatter.format(std::cout);
}

void Server::display_version()
{
    std::cout << YESS_NAME << " v" << YESS_VER << std::endl;
}

std::string Server::conn_str() { return conn_str_; }

int Server::main(const std::vector<std::string> &args)
{
    switch (requested_info_) {
    case Cli_info_opts::help:
        display_help();
        return Application::EXIT_OK;
        break;
    case Cli_info_opts::version:
        display_version();
        return Application::EXIT_OK;
        break;
    case Cli_info_opts::none:
        break;
    }

    // get parameters from configuration file
    unsigned short port = (unsigned short)config().getInt("Server.port", port_);
    port_ = port;
    log::info("Starting yess on port {}...", port);

    // set-up a server socket
    ServerSocket svs(port);
    // set-up a SocketReactor...
    SocketReactor reactor;
    // ... and a SocketAcceptor
    SocketAcceptor<Service_handler> acceptor(svs, reactor);
    // run the reactor in its own thread so that we can wait
    // for a termination request
    Thread thread;
    thread.start(reactor);
    // wait for CTRL-C or kill
    waitForTerminationRequest();
    // Stop the SocketReactor
    reactor.stop();
    thread.join();
    return Application::EXIT_OK;
}
