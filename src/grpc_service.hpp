#pragma once

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "../protos/yess.grpc.pb.h"
#include "action_handler.hpp"
#include "msg/response.hpp"
#include "nlohmann/json.hpp"
#include "yess.pb.h"

using grpc::Server;
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
class YessServiceImpl : public YessService::Service
{
  public:
    YessServiceImpl(std::string conn_str)
        : handler_(std::make_unique<yess::Action_handler>(conn_str))
    {
    }

  private:
    Status CreateStream(ServerContext *context,
                        const CreateStreamReq *request,
                        Response *reply) override
    {
        json cmd = {{"action", "CreateStream"}, {"type", request->type()}};

        msg::Response resp = handler_->handle(cmd);
        reply->set_msg(resp.getMessage());
        reply->set_status((int)resp.getStatus());
        return Status::OK;
    }

    Status PushEvent(ServerContext *context,
                     const PushEventReq *request,
                     Response *reply) override
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
    std::unique_ptr<yess::Action_handler> handler_;
};

void run_server()
{
    std::string server_address("0.0.0.0:2929");
    YessServiceImpl service("yess.db");

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}
} // namespace yess
