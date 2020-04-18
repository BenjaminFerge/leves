#ifndef YESS_COMMAND_HPP
#define YESS_COMMAND_HPP

#include "command_result.hpp"
namespace yess::cmd
{
class Command
{
  public:
    Command();
    virtual ~Command();
    virtual Command_result execute() = 0;
};
} // namespace yess::cmd

#endif // YESS_COMMAND_HPP
