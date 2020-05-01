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
using yess::YessService;
using json = nlohmann::json;

namespace yess
{
Grpc_service::Grpc_service(std::string conn_str)
    : handler_(std::make_unique<yess::Action_handler>(conn_str))
{
}

ResponseStatus* make_status(int code = 0, std::string msg = "OK")
{
    auto status = new ResponseStatus;
    status->set_msg("OK");
    status->set_status(0);
    return status;
}

Status Grpc_service::CreateStream(ServerContext* context,
                                  const CreateStreamReq* request,
                                  CreateStreamResp* reply)
{
    handler_->create_stream(request->type());
    auto status = make_status();
    reply->set_allocated_status(status);
    return Status::OK;
}

Status Grpc_service::PushEvent(ServerContext* context,
                               const PushEventReq* request,
                               PushEventResp* reply)
{
    db::Event event = {
        -1,
        request->event().stream_id(),
        request->event().type(),
        request->event().payload(),
        request->event().version(),
    };

    auto status = make_status();
    try {
        handler_->push_event(request->stream_id(), event);
    } catch (const std::exception& ex) {
        status->set_msg(ex.what());
        status->set_status(1);
    }
    reply->set_allocated_status(status);
    return Status::OK;
}

Status
Grpc_service::CreateProjection(::grpc::ServerContext* context,
                               const ::yess::CreateProjectionReq* request,
                               ::yess::CreateProjectionResp* response)
{
    if (request->type().empty()) {
        handler_->create_projection(request->data());
    } else {
        handler_->create_projection(request->data(), request->type());
    }
    auto status = make_status();
    response->set_allocated_status(status);
    return Status::OK;
}

Status
Grpc_service::DeleteProjection(::grpc::ServerContext* context,
                               const ::yess::DeleteProjectionReq* request,
                               ::yess::DeleteProjectionResp* response)
{
    auto status = make_status();
    try {
        handler_->delete_projection(request->id());
    } catch (const std::exception& ex) {
        status->set_msg(ex.what());
        status->set_status(1);
    }
    response->set_allocated_status(status);
    return Status::OK;
}

void set_values(const db::Stream from, ::yess::Stream* to)
{
    to->set_id(from.id);
    to->set_type(from.type);
    to->set_version(from.version);
}

void set_values(const db::Event from, ::yess::Event* to)
{
    to->set_id(from.id);
    to->set_stream_id(from.streamId);
    to->set_type(from.type);
    to->set_payload(from.payload);
    to->set_version(from.version);
}

void set_values(const db::Projection from, ::yess::Projection* to)
{
    to->set_id(from.id);
    to->set_type(from.type);
    to->set_data(from.data);
}

Status Grpc_service::GetAllStreams(grpc::ServerContext* context,
                                   const yess::GetAllStreamsReq* request,
                                   yess::GetAllStreamsResp* reply)
{
    std::vector<db::Stream> streams = handler_->get_all_streams();
    for (const auto& stream : streams) {
        auto s = reply->add_streams();
        set_values(stream, s);
    }
    return Status::OK;
}

Status Grpc_service::GetStreamsByType(grpc::ServerContext* context,
                                      const yess::GetStreamsByTypeReq* request,
                                      yess::GetStreamsByTypeResp* reply)
{
    std::vector<db::Stream> streams =
        handler_->get_streams_by_type(request->type());
    for (const auto& stream : streams) {
        auto s = reply->add_streams();
        set_values(stream, s);
    }
    return Status::OK;
}

Status Grpc_service::GetStream(grpc::ServerContext* context,
                               const yess::GetStreamReq* request,
                               yess::GetStreamResp* reply)
{
    std::optional<db::Stream> maybe_stream =
        handler_->get_stream(request->id());
    if (!maybe_stream.has_value()) {
        auto status = new ResponseStatus();
        status->set_msg("Stream not found");
        reply->set_allocated_status(status);
        return Status::OK;
    }
    db::Stream stream = maybe_stream.value();
    auto s = reply->mutable_stream();
    set_values(stream, s);
    return Status::OK;
}

Status
Grpc_service::GetEventsByStreamId(grpc::ServerContext* context,
                                  const yess::GetEventsByStreamIdReq* request,
                                  yess::GetEventsByStreamIdResp* reply)
{
    std::vector<db::Event> events =
        handler_->get_events_by_stream_id(request->id());
    for (const auto& event : events) {
        auto e = reply->add_events();
        set_values(event, e);
    }
    return Status::OK;
}

grpc::Status Grpc_service::GetEventsByStreamType(
    grpc::ServerContext* context,
    const yess::GetEventsByStreamTypeReq* request,
    yess::GetEventsByStreamTypeResp* reply)
{
    std::vector<db::Event> events =
        handler_->get_events_by_stream_type(request->type());
    for (const auto& event : events) {
        auto e = reply->add_events();
        set_values(event, e);
    }
    return Status::OK;
}

grpc::Status
Grpc_service::GetProjections(::grpc::ServerContext* context,
                             const ::yess::GetProjectionsReq* request,
                             ::yess::GetProjectionsResp* response)
{
    std::vector<db::Projection> projections;
    const std::string& type = request->type();
    if (type.empty()) {
        projections = handler_->get_all_projections();
    } else {
        projections = handler_->get_projections_by_type(type);
    }
    for (const auto& proj : projections) {
        auto p = response->add_projections();
        set_values(proj, p);
    }
    auto status = make_status();
    response->set_allocated_status(status);
    return Status::OK;
}

grpc::Status Grpc_service::Play(::grpc::ServerContext* context,
                                const ::yess::PlayReq* request,
                                ::yess::PlayResp* response)
{
    auto status = make_status();
    json state;
    const std::string& type = request->stream_type();
    try {
        if (type.empty()) {
            state = handler_->play_projection(request->projection_id(),
                                              request->initial_state(),
                                              request->stream_id(),
                                              "projection");
        } else {
            state = handler_->play_projection(request->projection_id(),
                                              request->initial_state(),
                                              request->stream_type(),
                                              "projection");
        }
    } catch (const std::exception& ex) {
        status->set_msg(ex.what());
        status->set_status(1);
    }
    response->set_allocated_status(status);
    response->set_state(state.dump());
    return Status::OK;
}
} // namespace yess
