#pragma once

#include <Poco/AutoPtr.h>
#include <Poco/Delegate.h>
#include <Poco/FIFOBuffer.h>
#include <Poco/Net/StreamSocket.h>
#include <memory>
#include <string>

#include "ActionHandler.hpp"
#include "Poco/Net/SocketNotification.h"
#include "Poco/Net/SocketReactor.h"
#include "Response.hpp"
#include "Server.hpp"
#include "db/Repositories/StreamRepository.hpp"

namespace yess
{
namespace db
{
class StreamRepository;
} // namespace db
} // namespace yess

namespace Poco
{
class Thread;

namespace Net
{
class ReadableNotification;
class ServerSocket;
class ShutdownNotification;
class SocketReactor;
class WritableNotification;
template <class ServiceHandler> class SocketAcceptor;
} // namespace Net
namespace Util
{
class Application;
class HelpFormatter;
class Option;
class OptionSet;
class ServerApplication;
} // namespace Util
template <class C, class N> class NObserver;
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

namespace yess
{
class ServiceHandler
/// I/O handler class. This class (un)registers handlers for I/O based on
/// data availability. To ensure non-blocking behavior and alleviate spurious
/// socket writability callback triggering when no data to be sent is available,
/// FIFO buffers are used. I/O FIFOBuffer sends notifications on transitions
/// from [1] non-readable (i.e. empty) to readable, [2] writable to non-writable
/// (i.e. full) and [3] non-writable (i.e. full) to writable.
/// Based on these notifications, the handler member functions react by
/// enabling/disabling respective reactor framework notifications.
{
  public:
    ServiceHandler(StreamSocket &socket, SocketReactor &reactor);
    ~ServiceHandler();
    void onFIFOOutReadable(bool &b);
    void onFIFOInWritable(bool &b);
    void onSocketReadable(const AutoPtr<ReadableNotification> &pNf);
    void onSocketWritable(const AutoPtr<WritableNotification> &pNf);
    void onSocketShutdown(const AutoPtr<ShutdownNotification> &pNf);
    Response generateResponse(std::string req);

  private:
    enum { BUFFER_SIZE = 1024 };
    StreamSocket m_socket;
    SocketReactor &m_reactor;
    FIFOBuffer m_fifoIn;
    FIFOBuffer m_fifoOut;
    ActionHandler m_actionHandler;
    std::unique_ptr<db::StreamRepository> m_streamRepository;
};
} // namespace yess
