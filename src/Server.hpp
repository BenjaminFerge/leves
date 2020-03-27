#pragma once

#include <Poco/Delegate.h>               // for delegate
#include <Poco/FIFOBuffer.h>             // for FIFOBuffer
#include <Poco/Util/ServerApplication.h> // for ServerApplication
#include <string>                        // for string
#include <vector>                        // for vector

#include "Poco/Util/Application.h"   // for Application
#include "Poco/Util/HelpFormatter.h" // for HelpFormatter (ptr only)
#include "Poco/Util/OptionSet.h"     // for OptionSet

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
enum CLInfoOption {
    none,
    help,
    version,
};
static constexpr const char *s_aboutString =
    "Yess is a server application for handling event sourced systems.";

class Server : public Poco::Util::ServerApplication
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
    void displayVersion();
    int main(const std::vector<std::string> &args);

  private:
    CLInfoOption m_requestedInfo;
    bool m_isConfigLoaded;
};
} // namespace yess
