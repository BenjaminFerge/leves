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

namespace yess
{
class Grpc_service : public YessService::Service
{
  public:
    Grpc_service(std::string conn_str);

  private:
    std::unique_ptr<yess::Action_handler> handler_;
    grpc::Status CreateStream(grpc::ServerContext *context,
                              const yess::CreateStreamReq *request,
                              yess::Response *reply) override;

    grpc::Status PushEvent(grpc::ServerContext *context,
                           const yess::PushEventReq *request,
                           yess::Response *reply) override;
};
} // namespace yess
