#include <Poco/Net/ServerSocket.h>                 // for ServerSocket
#include <Poco/Net/SocketAcceptor.h>               // for SocketAcceptor
#include <Poco/Net/SocketReactor.h>                // for SocketReactor
#include <Poco/Thread.h>                           // for Thread
#include <Poco/Util/Application.h>                 // for Application, Appli...
#include <Poco/Util/HelpFormatter.h>               // for HelpFormatter
#include <Poco/Util/LayeredConfiguration.h>        // for LayeredConfiguration
#include <Poco/Util/Option.h>                      // for Option
#include <Poco/Util/OptionSet.h>                   // for OptionSet
#include <Poco/Util/ServerApplication.h>           // for ServerApplication
#include <iostream>                                // for operator<<, basic_...

#include "Server.hpp"
#include "ServiceHandler.hpp"                      // for ServiceHandler
#include "Version.h"                               // for YESS_NAME, YESS_VER
#include "db/Repositories/StreamRepository.hpp"    // for StreamRepository
#include "db/Repositories/../Entities/Stream.hpp"  // for db

using namespace yess;
using namespace db;

Server::Server() : m_requestedInfo(CLInfoOption::none), m_isConfigLoaded(false)
{
    std::string connectorKey =
        (std::string)config().getString("EventStore.ConnectorKey", "SQLite");
    std::string connectionString = (std::string)config().getString(
        "EventStore.ConnectionString", "yess.db");
    auto streamRepository = StreamRepository(connectorKey, connectionString);
    streamRepository.initDB();
}

Server::~Server() {}

void Server::initialize(Application &self)
{
    // load default configuration files, if present
    m_isConfigLoaded = loadConfiguration();
    ServerApplication::initialize(self);
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
}

void Server::handleOption(const std::string &name, const std::string &value)
{
    ServerApplication::handleOption(name, value);

    if (name == "help")
        m_requestedInfo = CLInfoOption::help;
    else if (name == "version") {
        m_requestedInfo = CLInfoOption::version;
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
    std::cout << "Starting yess..." << std::endl;

    // get parameters from configuration file
    unsigned short port = (unsigned short)config().getInt("Server.port", 9977);

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
