#ifndef YESS_SHELL_COMMANDS_HPP
#define YESS_SHELL_COMMANDS_HPP

#include "command.hpp"

namespace yess::cmd
{
class Unknown : public Command
{
  public:
    Unknown();
    Command_result execute() override;
};
class Help : public Command
{
  public:
    Help();
    Command_result execute() override;
};
class None : public Command
{
  public:
    None();
    Command_result execute() override;
};
class Quit : public Command
{
  public:
    Quit();
    Command_result execute() override;
};
} // namespace yess::cmd

#endif // YESS_SHELL_COMMANDS_HPP
