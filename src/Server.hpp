#pragma once

#include <string>
#include <vector>

#include "Poco/Util/Application.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/ServerApplication.h"

using Poco::Util::Application;
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
    std::string getConnStr();

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
    std::string m_connStr;
    unsigned short m_port = 2929;
};
} // namespace yess
