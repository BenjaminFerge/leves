#include "domain_commands.hpp"

using namespace yess::cmd;

Create_stream_req::Create_stream_req(const std::string& type) : type_(type)
{
}

Create_stream::Create_stream(const Action_handler& handler,
                             const Create_stream_req& request)
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

std::string Create_stream::usage()
{
    return std::string("create_stream T");
}

Domain_command::Domain_command(const yess::Action_handler& handler)
    : handler_(handler)
{
}

Push_req::Push_req(const int& stream_id,
                   std::string type,
                   std::string payload,
                   const int& version)
    : stream_id_(stream_id), type_(std::move(type)),
      payload_(std::move(payload)), version_(version)
{
}

Push::Push(const yess::Action_handler& handler, const Push_req& req)
    : Domain_command(handler), request_(req)
{
}

Command_result Push::execute()
{
    db::Event e{
        -1,
        request_.stream_id_,
        request_.type_,
        request_.payload_,
        request_.version_,
    };
    try {
        handler_.push_event(request_.stream_id_, e);
    } catch (std::runtime_error err) {
        return Command_result(
            Command_result::Status::error, err.what(), nullptr);
    } catch (std::exception /* ex */) {
        std::string msg = "Unexpected error: cannot push event";
        return Command_result(Command_result::Status::error, msg, nullptr);
    }
    return Command_result(Command_result::Status::ok, "OK", std::any());
}

std::string Push::usage()
{
    return std::string("push SID T JSON VER");
}
Create_projection_req::Create_projection_req(std::string data, std::string type)
    : data(data), type(type)
{
}
Create_projection_req::Create_projection_req(std::string data) : data(data)
{
}
Create_projection::Create_projection(const yess::Action_handler& handler,
                                     const Create_projection_req& req)
    : Domain_command(handler), request_(req)
{
}
Command_result Create_projection::execute()
{
    try {
        if (!request_.type.empty()) {
            handler_.create_projection(request_.data, request_.type);
        } else {
            handler_.create_projection(request_.data);
        }
    } catch (std::runtime_error err) {
        return Command_result(
            Command_result::Status::error, err.what(), nullptr);
    }
    return Command_result(
        Command_result::Status::ok, "OK", std::any());
}
std::string Create_projection::usage()
{
    return std::string("create_projection JS [T]");
}
