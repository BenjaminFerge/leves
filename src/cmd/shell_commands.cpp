#include "shell_commands.hpp"
#include "domain_commands.hpp"

#define SH_HELP_MSG                                                            \
    "help", "show this help message", "quit", "leave the interactive shell",   \
        Push::usage().c_str(), "push event into stream",                       \
        Create_stream::usage().c_str(), "create a new event stream"

yess::cmd::Command_result yess::cmd::Help::execute()
{
    char* tmp;
    char* buf;
    size_t sz;
    const char* format = "%-25s%s\n"
                         "%-25s%s\n"
                         "%-25s%s\n"
                         "%-25s%s";
    sz = snprintf(nullptr, 0, format, SH_HELP_MSG);
    buf = (char*)malloc(sz + 1);
    if (buf == nullptr) {
        throw std::runtime_error(
            "Cannot allocate memory for the help message.");
    }
    snprintf(buf, sz + 1, format, SH_HELP_MSG);

    std::string msg(buf);
    return yess::cmd::Command_result(
        yess::cmd::Command_result::Status::ok, msg, std::any());
}
yess::cmd::Help::Help()
{
}
yess::cmd::Command_result yess::cmd::Quit::execute()
{
    return yess::cmd::Command_result(
        yess::cmd::Command_result::Status::exit, "Bye!", std::any());
}
yess::cmd::Quit::Quit()
{
}
yess::cmd::Unknown::Unknown()
{
}
yess::cmd::Command_result yess::cmd::Unknown::execute()
{
    return yess::cmd::Command_result(
        yess::cmd::Command_result::Status::error,
        "Unknown command. Enter 'help' for usage hints.",
        std::any());
}
yess::cmd::None::None()
{
}
yess::cmd::Command_result yess::cmd::None::execute()
{
    return yess::cmd::Command_result(
        yess::cmd::Command_result::Status::ok, "", std::any());
}

yess::cmd::Command_result yess::cmd::Invalid::execute()
{
    return yess::cmd::Command_result(
        yess::cmd::Command_result::Status::error, prefix_ + usage_, std::any());
}
yess::cmd::Invalid::Invalid(std::string usage) : usage_(std::move(usage))
{
}
