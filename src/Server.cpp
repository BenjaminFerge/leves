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
#include <iostream>
#include <string>

#include "Poco/Foundation.h"
#include "Poco/Net/Net.h"
#include "Poco/Util/Subsystem.h"
#include "Server.hpp"
#include "ServiceHandler.hpp"
#include "Version.h"
#include "db/Repositories/SqliteStreamRepo.hpp"
#include "log.hpp"

using namespace yess;
using namespace db;
using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;

Server::Server() : m_requestedInfo(CLInfoOption::none), m_isConfigLoaded(false)
{
}

Server::~Server() {}

void Server::initDB()
{
    std::string connectorKey =
        (std::string)config().getString("EventStore.ConnectorKey", "SQLite");
    std::string connectionString = (std::string)config().getString(
        "EventStore.ConnectionString", "yess.db");

    if (!m_connStr.empty()) {
        connectionString = m_connStr;
    }
    auto streamRepository = SqliteStreamRepo(connectorKey, connectionString);
    streamRepository.initDB();
}

void Server::initialize(Application &self)
{
    // load default configuration files, if present
    m_isConfigLoaded = loadConfiguration();
    ServerApplication::initialize(self);
    initDB();
}

void Server::uninitialize() { ServerApplication::uninitialize(); }

void Server::defineOptions(OptionSet &options)
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

void Server::handleOption(const std::string &name, const std::string &value)
{
    ServerApplication::handleOption(name, value);

    if (name == "help")
        m_requestedInfo = CLInfoOption::help;
    else if (name == "version")
        m_requestedInfo = CLInfoOption::version;
    else if (name == "conn-str")
        m_connStr = value;
    else if (name == "log-path")
        m_logPath = value;

    if (!m_logPath.empty()) {
        log::info("Logging path is set to: {}", m_logPath);
        log::setFileLogger(m_logPath);
    }
}

void Server::displayHelp()
{
    HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.setHeader(s_aboutString);
    helpFormatter.format(std::cout);
}

void Server::displayVersion()
{
    std::cout << YESS_NAME << " v" << YESS_VER << std::endl;
}

std::string Server::getConnStr() { return m_connStr; }

int Server::main(const std::vector<std::string> &args)
{
    switch (m_requestedInfo) {
    case CLInfoOption::help:
        displayHelp();
        return Application::EXIT_OK;
        break;
    case CLInfoOption::version:
        displayVersion();
        return Application::EXIT_OK;
        break;
    case CLInfoOption::none:
        break;
    }

    // get parameters from configuration file
    unsigned short port =
        (unsigned short)config().getInt("Server.port", m_port);
    m_port = port;
    log::info("Starting yess on port {}...", port);

    // set-up a server socket
    ServerSocket svs(port);
    // set-up a SocketReactor...
    SocketReactor reactor;
    // ... and a SocketAcceptor
    SocketAcceptor<ServiceHandler> acceptor(svs, reactor);
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
