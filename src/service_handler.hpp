#pragma once

#include <memory>
#include <string>

#include "Poco/AutoPtr.h"
#include "Poco/FIFOBuffer.h"
#include "Poco/Net/SocketNotification.h"
#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/StreamSocket.h"

#include "action_handler.hpp"
#include "db/repositories/stream_repository.hpp"
#include "response.hpp"

using Poco::AutoPtr;
using Poco::FIFOBuffer;
using Poco::Net::ReadableNotification;
using Poco::Net::ShutdownNotification;
using Poco::Net::SocketReactor;
using Poco::Net::StreamSocket;
using Poco::Net::WritableNotification;

namespace yess
{
class Service_handler
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
    Service_handler(StreamSocket &socket, SocketReactor &reactor);
    ~Service_handler();
    void onFIFOOutReadable(bool &b);
    void onFIFOInWritable(bool &b);
    void onSocketReadable(const AutoPtr<ReadableNotification> &pNf);
    void onSocketWritable(const AutoPtr<WritableNotification> &pNf);
    void onSocketShutdown(const AutoPtr<ShutdownNotification> &pNf);
    Response generate_response(std::string req);

  private:
    enum { BUFFER_SIZE = 1024 };
    StreamSocket socket_;
    SocketReactor &reactor_;
    FIFOBuffer fifo_in_;
    FIFOBuffer fifo_out_;
    Action_handler action_handler_;
    std::unique_ptr<db::Stream_repository> stream_repo_;
};
} // namespace yess
