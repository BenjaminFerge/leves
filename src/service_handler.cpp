#include <memory>
#include <string>

#include "Poco/AutoPtr.h"
#include "Poco/Exception.h"
#include "Poco/Foundation.h"
#include <Poco/AbstractDelegate.h>
#include <Poco/AbstractEvent.h>
#include <Poco/BasicEvent.h>
#include <Poco/Delegate.h>
#include <Poco/FIFOBuffer.h>
#include <Poco/Logger.h>
#include <Poco/NObserver.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/SocketNotification.h>
#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Util/Application.h>
#include <nlohmann/json.hpp>

#include "action_handler.hpp"
#include "db/entities/event.hpp"
#include "response.hpp"
#include "server.hpp"
#include "service_handler.hpp"

using namespace yess;
using namespace db;
using namespace Poco;
using json = nlohmann::json;

Service_handler::Service_handler(StreamSocket &socket, SocketReactor &reactor)
    : socket_(socket), reactor_(reactor), fifo_in_(BUFFER_SIZE, true),
      fifo_out_(BUFFER_SIZE, true)
{
    Application &app = Application::instance();
    app.logger().information("Connection from " +
                             socket.peerAddress().toString());

    reactor_.addEventHandler(socket_,
                             NObserver<Service_handler, ReadableNotification>(
                                 *this, &Service_handler::onSocketReadable));
    reactor_.addEventHandler(socket_,
                             NObserver<Service_handler, ShutdownNotification>(
                                 *this, &Service_handler::onSocketShutdown));

    fifo_out_.readable += delegate(this, &Service_handler::onFIFOOutReadable);
    fifo_in_.writable += delegate(this, &Service_handler::onFIFOInWritable);
}

Service_handler::~Service_handler()
{
    Application &app = Application::instance();
    try {
        app.logger().information("Disconnecting " +
                                 socket_.peerAddress().toString());
    } catch (...) {
    }
    reactor_.removeEventHandler(
        socket_,
        NObserver<Service_handler, ReadableNotification>(
            *this, &Service_handler::onSocketReadable));
    reactor_.removeEventHandler(
        socket_,
        NObserver<Service_handler, WritableNotification>(
            *this, &Service_handler::onSocketWritable));
    reactor_.removeEventHandler(
        socket_,
        NObserver<Service_handler, ShutdownNotification>(
            *this, &Service_handler::onSocketShutdown));

    fifo_out_.readable -= delegate(this, &Service_handler::onFIFOOutReadable);
    fifo_in_.writable -= delegate(this, &Service_handler::onFIFOInWritable);
}

void Service_handler::onFIFOOutReadable(bool &b)
{
    if (b)
        reactor_.addEventHandler(
            socket_,
            NObserver<Service_handler, WritableNotification>(
                *this, &Service_handler::onSocketWritable));
    else
        reactor_.removeEventHandler(
            socket_,
            NObserver<Service_handler, WritableNotification>(
                *this, &Service_handler::onSocketWritable));
}

void Service_handler::onFIFOInWritable(bool &b)
{
    if (b)
        reactor_.addEventHandler(
            socket_,
            NObserver<Service_handler, ReadableNotification>(
                *this, &Service_handler::onSocketReadable));
    else
        reactor_.removeEventHandler(
            socket_,
            NObserver<Service_handler, ReadableNotification>(
                *this, &Service_handler::onSocketReadable));
}

Response Service_handler::generate_response(std::string req)
{
    json obj;
    try {
        obj = json::parse(req);
    } catch (Poco::Exception &exc) {
        return Response(ResponseStatus::Error, exc.displayText());
    }
    return action_handler_.handle(obj);
}

void Service_handler::onSocketReadable(const AutoPtr<ReadableNotification> &pNf)
{
    // some socket implementations (windows) report available
    // bytes on client disconnect, so we  double-check here
    if (socket_.available()) {
        int len = socket_.receiveBytes(fifo_in_);
        std::string req(fifo_in_.begin());
        req.resize(len);
        Response resp = generate_response(req);
        std::string msg = resp.getMessage();
        fifo_out_.copy(msg.c_str(), msg.length());
        fifo_in_.drain(fifo_in_.size());
    }
}

void Service_handler::onSocketWritable(const AutoPtr<WritableNotification> &pNf)
{
    socket_.sendBytes(fifo_out_);
}

void Service_handler::onSocketShutdown(const AutoPtr<ShutdownNotification> &pNf)
{
    delete this;
}
