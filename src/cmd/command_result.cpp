#include "command_result.hpp"
namespace yess::cmd
{
Command_result::Command_result(Status status,
                               const std::string& msg,
                               std::any data)
    : status_(status), msg_(msg), data_(data)
{
}
std::string Command_result::message()
{
    return msg_;
}
Command_result::Status Command_result::status()
{
    return status_;
}
std::any Command_result::data()
{
    return data_;
}
bool Command_result::ok()
{
    return status_ == Status::ok;
}
} // namespace yess::cmd
