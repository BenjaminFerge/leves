#include <iostream>

#include <Poco/Delegate.h>
#include <Poco/FIFOBuffer.h>
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

#include "Server.hpp"
#include "ServiceHandler.hpp"

using Poco::AutoPtr;
using Poco::delegate;
using Poco::FIFOBuffer;
using Poco::NObserver;
using Poco::Thread;
using Poco::Net::ReadableNotification;
using Poco::Net::ServerSocket;
using Poco::Net::ShutdownNotification;
using Poco::Net::SocketAcceptor;
using Poco::Net::SocketReactor;
using Poco::Net::StreamSocket;
using Poco::Net::WritableNotification;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

Server::Server() : m_helpRequested(false) {}

Server::~Server() {}

void Server::initialize(Application &self)
{
    loadConfiguration(); // load default configuration files, if
                         // present
    ServerApplication::initialize(self);
}

void Server::uninitialize() { ServerApplication::uninitialize(); }

void Server::defineOptions(OptionSet &options)
{
    ServerApplication::defineOptions(options);

    options.addOption(
        Option(
            "help", "h", "display help information on command line arguments")
            .required(false)
            .repeatable(false));
}

void Server::handleOption(const std::string &name, const std::string &value)
{
    ServerApplication::handleOption(name, value);

    if (name == "help")
        m_helpRequested = true;
}

void Server::displayHelp()
{
    HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.setHeader(
        "An echo server implemented using the Reactor and Acceptor "
        "patterns.");
    helpFormatter.format(std::cout);
}

int Server::main(const std::vector<std::string> &args)
{
    if (m_helpRequested) {
        displayHelp();
    } else {
        // get parameters from configuration file
        unsigned short port =
            (unsigned short)config().getInt("Server.port", 9977);

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
    }
    return Application::EXIT_OK;
}
