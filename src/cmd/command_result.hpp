#ifndef YESS_COMMAND_RESULT_HPP
#define YESS_COMMAND_RESULT_HPP

#include <any>
#include <string>

namespace yess::cmd
{
class Command_result
{
  public:
    enum class Status { ok, error, exit };
    Command_result(Status status, const std::string &msg, std::any data);
    std::string message();
    Status status();
    std::any data();
    bool ok();

  private:
    Status status_;
    std::string msg_;
    std::any data_;
};
} // namespace yess::cmd
#endif // YESS_COMMAND_RESULT_HPP
