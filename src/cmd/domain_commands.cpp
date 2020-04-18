#include "domain_commands.hpp"

using namespace yess::cmd;

Create_stream_req::Create_stream_req(const std::string &type) : type_(type) {}

Create_stream::Create_stream(const Action_handler &handler,
                             const Create_stream_req &request)
    : Domain_command(handler), request_(request)
{
}

Command_result Create_stream::execute()
{
    try {
        handler_.create_stream(request_.type_);
    } catch (std::exception /* ex */) {
        std::string msg = "Unexpected error: cannot create stream";
        return Command_result(Command_result::Status::error, msg, nullptr);
    }
    return Command_result(Command_result::Status::ok, "OK", nullptr);
}

Domain_command::Domain_command(const yess::Action_handler &handler)
    : handler_(handler)
{
}
