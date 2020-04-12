#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "../protos/yess.grpc.pb.h"
#include "action_handler.hpp"
#include "grpc_service.hpp"
#include "msg/response.hpp"
#include "nlohmann/json.hpp"
#include "yess.pb.h"

using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using yess::CreateStreamReq;
using yess::PushEventReq;
using yess::Response;
using yess::YessService;
using json = nlohmann::json;

namespace yess
{
Grpc_service::Grpc_service(std::string conn_str)
    : handler_(std::make_unique<yess::Action_handler>(conn_str))
{
}

Status Grpc_service::CreateStream(ServerContext *context,
                                  const CreateStreamReq *request,
                                  Response *reply)
{
    json cmd = {{"action", "CreateStream"}, {"type", request->type()}};

    msg::Response resp = handler_->handle(cmd);
    reply->set_msg(resp.getMessage());
    reply->set_status((int)resp.getStatus());
    return Status::OK;
}

Status Grpc_service::PushEvent(ServerContext *context,
                               const PushEventReq *request,
                               Response *reply)
{

    json cmd = {{"action", "PushEvent"},
                {"streamId", request->stream_id()},
                {"type", request->event().type()},
                {"payload", request->event().payload()},
                {"version", request->event().version()}};

    msg::Response resp = handler_->handle(cmd);
    reply->set_msg(resp.getMessage());
    reply->set_status((int)resp.getStatus());
    return Status::OK;
}
} // namespace yess
