#pragma once

#include "action_handler.hpp"
#include <memory>
#include <string>
#include <vector>

namespace yess
{
enum Cli_info_opts {
    none,
    help,
    version,
};
static constexpr const char *about_str =
    "Yess is a server application for handling event sourced systems.";

class Server
{
  public:
    Server();
    ~Server();
    std::string conn_str();
    void initDB();
    int run();

  protected:
    std::string conn_str_ = "yess.db";
    std::unique_ptr<Action_handler> handler_;
    unsigned short port_ = 2929;
    void handle_option(const std::string &name, const std::string &value);
    void display_help();
    void display_version();

  private:
    Cli_info_opts requested_info_;
    bool is_config_loaded_;
    std::string log_path_;
};
} // namespace yess
