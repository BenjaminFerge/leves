#include "shell_commands.hpp"
yess::cmd::Command_result yess::cmd::Help::execute()
{
    return yess::cmd::Command_result(yess::cmd::Command_result::Status::ok,
                                     "A useful help message...\n",
                                     std::any());
}
yess::cmd::Help::Help() {}
yess::cmd::Command_result yess::cmd::Quit::execute()
{
    return yess::cmd::Command_result(
        yess::cmd::Command_result::Status::exit, "Bye!\n", std::any());
}
yess::cmd::Quit::Quit() {}
yess::cmd::Unknown::Unknown() {}
yess::cmd::Command_result yess::cmd::Unknown::execute()
{
    return yess::cmd::Command_result(
        yess::cmd::Command_result::Status::error,
        "Unknown command. Enter 'help' for usage hints.\n",
        std::any());
}
yess::cmd::None::None() {}
yess::cmd::Command_result yess::cmd::None::execute()
{
    return yess::cmd::Command_result(yess::cmd::Command_result::Status::ok,
                                     "",
                                     std::any());
}
