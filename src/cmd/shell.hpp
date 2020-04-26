#ifndef YESS_SHELL_HPP
#define YESS_SHELL_HPP

#include "../action_handler.hpp"
#include "command.hpp"

namespace yess::cmd
{
class Shell
{
  public:
    enum class Shell_cmd {
        none,
        unknown,
        invalid,
        quit,
        help,
        create_stream,
        push,
        play,
        create_projection,
        get_streams,
        get_projections,
    };
    Shell(const Action_handler& handler);
    void run();

  private:
    std::tuple<Shell_cmd, std::vector<std::string>> tokens(std::string in);
    const Action_handler& handler_;
    std::unique_ptr<Command> interpret(std::string in);
    void execute(const Command& cmd);
    std::string prompt_ = "> ";
};
} // namespace yess::cmd

#endif // YESS_SHELL_HPP
