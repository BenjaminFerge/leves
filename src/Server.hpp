#pragma once

#include <Poco/Delegate.h>
#include <Poco/FIFOBuffer.h>
#include <Poco/Util/ServerApplication.h>
#include <string>
#include <vector>

namespace Poco
{
class Thread;

namespace Net
{
class ReadableNotification;
class ServerSocket;
class ShutdownNotification;
class SocketReactor;
class StreamSocket;
class WritableNotification;
template <class ServiceHandler> class SocketAcceptor;
} // namespace Net
namespace Util
{
class Application;
class HelpFormatter;
class Option;
class OptionSet;
} // namespace Util
template <class C, class N> class NObserver;
template <class C> class AutoPtr;
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

class Server : public Poco::Util::ServerApplication
/// The main application class.
///
/// This class handles command-line arguments and
/// configuration files.
/// Start the Server executable with the help
/// option (/help on Windows, --help on Unix) for
/// the available command line options.
///
/// To use the sample configuration file (Server.properties),
/// copy the file to the directory where the Server executable
/// resides. If you start the debug version of the Server
/// (EchoServerd[.exe]), you must also create a copy of the configuration
/// file named EchoServerd.properties. In the configuration file, you
/// can specify the port on which the server is listening (default
/// 9977) and the format of the date/time string sent back to the client.
///
/// To test the Server you can use any telnet client (telnet localhost
/// 9977).
{
  public:
    Server();
    ~Server();

  protected:
    void initialize(Application &self);
    void uninitialize();
    void defineOptions(OptionSet &options);
    void handleOption(const std::string &name, const std::string &value);
    void displayHelp();
    int main(const std::vector<std::string> &args);

  private:
    bool m_helpRequested;
};
