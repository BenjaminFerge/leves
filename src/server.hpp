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
enum Cli_info_opts {
    none,
    help,
    version,
};
static constexpr const char *about_str =
    "Yess is a server application for handling event sourced systems.";

class Server : public Poco::Util::ServerApplication
{
  public:
    Server();
    ~Server();
    std::string conn_str();
    void initDB();

  protected:
    void initialize(Application &self);
    void uninitialize();
    void define_options(OptionSet &options);
    void handle_option(const std::string &name, const std::string &value);
    void display_help();
    void display_version();
    int main(const std::vector<std::string> &args);

  private:
    Cli_info_opts requested_info_;
    bool is_config_loaded_;
    std::string conn_str_;
    std::string log_path_;
    unsigned short port_ = 2929;
};
} // namespace yess
