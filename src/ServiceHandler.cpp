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
#include <string>

#include "ServiceHandler.hpp"

namespace Poco
{
class Thread;
namespace Net
{
class ServerSocket;
template <class ServiceHandler> class SocketAcceptor;
} // namespace Net
namespace Util
{
class HelpFormatter;
class Option;
class OptionSet;
class ServerApplication;
} // namespace Util
} // namespace Poco

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

using namespace Leves;

ServiceHandler::ServiceHandler(StreamSocket &socket, SocketReactor &reactor)
    : m_socket(socket), m_reactor(reactor), m_fifoIn(BUFFER_SIZE, true),
      m_fifoOut(BUFFER_SIZE, true)
{
    Application &app = Application::instance();
    app.logger().information("Connection from " +
                             socket.peerAddress().toString());

    m_reactor.addEventHandler(m_socket,
                              NObserver<ServiceHandler, ReadableNotification>(
                                  *this, &ServiceHandler::onSocketReadable));
    m_reactor.addEventHandler(m_socket,
                              NObserver<ServiceHandler, ShutdownNotification>(
                                  *this, &ServiceHandler::onSocketShutdown));

    m_fifoOut.readable += delegate(this, &ServiceHandler::onFIFOOutReadable);
    m_fifoIn.writable += delegate(this, &ServiceHandler::onFIFOInWritable);
}

ServiceHandler::~ServiceHandler()
{
    Application &app = Application::instance();
    try {
        app.logger().information("Disconnecting " +
                                 m_socket.peerAddress().toString());
    } catch (...) {
    }
    m_reactor.removeEventHandler(
        m_socket,
        NObserver<ServiceHandler, ReadableNotification>(
            *this, &ServiceHandler::onSocketReadable));
    m_reactor.removeEventHandler(
        m_socket,
        NObserver<ServiceHandler, WritableNotification>(
            *this, &ServiceHandler::onSocketWritable));
    m_reactor.removeEventHandler(
        m_socket,
        NObserver<ServiceHandler, ShutdownNotification>(
            *this, &ServiceHandler::onSocketShutdown));

    m_fifoOut.readable -= delegate(this, &ServiceHandler::onFIFOOutReadable);
    m_fifoIn.writable -= delegate(this, &ServiceHandler::onFIFOInWritable);
}

void ServiceHandler::onFIFOOutReadable(bool &b)
{
    if (b)
        m_reactor.addEventHandler(
            m_socket,
            NObserver<ServiceHandler, WritableNotification>(
                *this, &ServiceHandler::onSocketWritable));
    else
        m_reactor.removeEventHandler(
            m_socket,
            NObserver<ServiceHandler, WritableNotification>(
                *this, &ServiceHandler::onSocketWritable));
}

void ServiceHandler::onFIFOInWritable(bool &b)
{
    if (b)
        m_reactor.addEventHandler(
            m_socket,
            NObserver<ServiceHandler, ReadableNotification>(
                *this, &ServiceHandler::onSocketReadable));
    else
        m_reactor.removeEventHandler(
            m_socket,
            NObserver<ServiceHandler, ReadableNotification>(
                *this, &ServiceHandler::onSocketReadable));
}

void ServiceHandler::onSocketReadable(const AutoPtr<ReadableNotification> &pNf)
{
    // some socket implementations (windows) report available
    // bytes on client disconnect, so we  double-check here
    if (m_socket.available()) {
        int len = m_socket.receiveBytes(m_fifoIn);
        m_fifoIn.drain(m_fifoOut.write(m_fifoIn.buffer()));
    }
}

void ServiceHandler::onSocketWritable(const AutoPtr<WritableNotification> &pNf)
{
    m_socket.sendBytes(m_fifoOut);
}

void ServiceHandler::onSocketShutdown(const AutoPtr<ShutdownNotification> &pNf)
{
    delete this;
}
