#include <iostream>
#include <string>
#include <vector>

#include <Poco/Delegate.h>
#include <Poco/Exception.h>
#include <Poco/FIFOBuffer.h>
#include <Poco/NObserver.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/SocketAcceptor.h>
#include <Poco/Net/SocketNotification.h>
#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Thread.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/ServerApplication.h>

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
