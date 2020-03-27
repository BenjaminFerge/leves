#pragma once

#include <Poco/AutoPtr.h>                        // for AutoPtr
#include <Poco/Delegate.h>                       // for delegate
#include <Poco/FIFOBuffer.h>                     // for FIFOBuffer
#include <Poco/Net/StreamSocket.h>               // for StreamSocket
#include <memory>                                // for unique_ptr
#include <string>                                // for string

#include "ActionHandler.hpp"                     // for ActionHandler
#include "Poco/Net/SocketNotification.h"         // for ReadableNotification
#include "Poco/Net/SocketReactor.h"              // for SocketReactor
#include "Response.hpp"                          // for Response
#include "Server.hpp"                            // for FIFOBuffer, AutoPtr
#include "db/Repositories/StreamRepository.hpp"  // for StreamRepository
#include "Poco/Util/ServerApplication.h"         // for ServerApplication
#include "Poco/Util/Application.h"               // for Application, OptionSet

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
