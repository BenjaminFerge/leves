#pragma once

#include <memory>
#include <string>
#include <vector>

#include <argparse.hpp>

namespace yess
{
static constexpr const char *about_str =
    "Yess is a server application for handling event sourced systems.";

class Server
{
  public:
    Server();
    Server(int argc, char **argv);
    ~Server();
    std::string conn_str();
    void initDB();
    int run();

  protected:
    std::string conn_str_ = "yess.db";
    unsigned short port_ = 2929;
    void display_version();

  private:
    void parse_args(int argc, char **argv);
    bool is_config_loaded_ = false;
    bool daemon_ = false;
    std::string log_path_;
    argparse::ArgumentParser argparser_;
    void daemon();
};
} // namespace yess
