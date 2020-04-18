#ifndef YESS_DOMAIN_COMMANDS_HPP
#define YESS_DOMAIN_COMMANDS_HPP

#include "command.hpp"
#include "../action_handler.hpp"
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
struct Create_stream_req
{
    explicit Create_stream_req(const std::string& type);
    std::string type_;
};

class Create_stream : public Domain_command
{
    const Create_stream_req &request_;

  public:
    Create_stream(const Action_handler& handler, const Create_stream_req& req);
    Command_result execute() override;
};
} // namespace yess::cmd

#endif // YESS_DOMAIN_COMMANDS_HPP
