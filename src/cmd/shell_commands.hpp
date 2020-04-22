#ifndef YESS_SHELL_COMMANDS_HPP
#define YESS_SHELL_COMMANDS_HPP

#include "command.hpp"
#include <memory>

namespace yess::cmd
{
class Shell_command : public Command
{
};
class Unknown : public Shell_command
{
  public:
    Unknown();
    Command_result execute() override;
};
class Help : public Shell_command
{
  public:
    Help();
    Command_result execute() override;
};
class None : public Shell_command
{
  public:
    None();
    Command_result execute() override;
};
class Invalid : public Shell_command
{
  public:
    Invalid(std::string usage);
    Command_result execute() override;

  private:
    std::string usage_;
    const std::string prefix_ = "Invalid command.\nUsage: ";
};
class Quit : public Shell_command
{
  public:
    Quit();
    Command_result execute() override;
};
} // namespace yess::cmd

#endif // YESS_SHELL_COMMANDS_HPP
