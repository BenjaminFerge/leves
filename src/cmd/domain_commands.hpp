#ifndef YESS_DOMAIN_COMMANDS_HPP
#define YESS_DOMAIN_COMMANDS_HPP

#include "../action_handler.hpp"
#include "command.hpp"
#include <string>

namespace yess::cmd
{
class Domain_command : public Command
{
  public:
    explicit Domain_command(const Action_handler &handler);

  protected:
    const Action_handler &handler_;
};
struct Create_stream_req {
    explicit Create_stream_req(const std::string &type);
    std::string type_;
};

class Create_stream : public Domain_command
{
  public:
    Create_stream(const Action_handler &handler, const Create_stream_req &req);
    Command_result execute() override;

  private:
    const Create_stream_req &request_;
};

struct Push_req {
    explicit Push_req(const int &stream_id,
                      std::string type,
                      std::string payload,
                      const int &version);
    int stream_id_;
    std::string type_;
    std::string payload_;
    int version_;
};

class Push : public Domain_command
{
  public:
    Push(const Action_handler &handler, const Push_req &req);
    Command_result execute() override;

  private:
    const Push_req &request_;
};
} // namespace yess::cmd

#endif // YESS_DOMAIN_COMMANDS_HPP
