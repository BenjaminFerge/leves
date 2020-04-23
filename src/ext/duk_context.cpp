#include <stdexcept>
#include <string>
#include <vector>

#include "../db/entities/event.hpp"
#include "../log.hpp"
#include "duk_context.hpp"
#include "duktape.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace yess::ext
{
Duk_context::Duk_context()
{
    //
    ctx_ = duk_create_heap_default();
}

Duk_context::~Duk_context()
{
    // FIXME: causes segfault
    // duk_destroy_heap(ctx_);
}

duk_context* Duk_context::get_duk_ctx()
{
    return ctx_;
}

void Duk_context::read(const std::string& body)
{
    duk_push_string(ctx_, body.c_str());
    if (duk_peval(ctx_) != 0) {
        duk_get_prop_string(ctx_, -1, "stack");
        throw std::runtime_error(duk_safe_to_string(ctx_, -1));
    }
    duk_pop(ctx_); // ignore result
    duk_push_global_object(ctx_);
}

json Duk_context::call_projection(const std::string& fn_name,
                                  const std::vector<db::Event>& events,
                                  json init_state)
{
    json state = init_state;
    if (events.size() == 0) {
        yess::log::warn("The called projection has no events");
        return state;
    }
    duk_idx_t objIdx = 0;
    duk_idx_t fnIdx = 0;

    if (!duk_is_valid_index(ctx_, objIdx)) {
        throw std::runtime_error("Invalid index!");
    }
    if (duk_get_prop_string(ctx_, objIdx, fn_name.c_str()) == 0) {
        throw std::runtime_error("Function '" + fn_name + "' is not found!");
    }
    if (duk_get_prop_string(ctx_, objIdx, fn_name.c_str()) == 0) {
        throw std::runtime_error("Function '" + fn_name + "' is not found!");
    }

    // Fill parameters
    for (const auto& event : events) {
        // param 1: event
        duk_idx_t evIdx = duk_push_object(ctx_);

        duk_push_int(ctx_, event.id);
        duk_put_prop_string(ctx_, evIdx, "id");

        duk_push_string(ctx_, event.payload.c_str());
        duk_json_decode(ctx_, -1);
        duk_put_prop_string(ctx_, evIdx, "payload");

        duk_push_int(ctx_, event.version);
        duk_put_prop_string(ctx_, evIdx, "version");

        duk_push_int(ctx_, event.streamId);
        duk_put_prop_string(ctx_, evIdx, "streamId");

        duk_push_string(ctx_, event.type.c_str());
        duk_put_prop_string(ctx_, evIdx, "type");

        // param 2: state
        std::string jsonState = state.dump();
        duk_push_string(ctx_, jsonState.c_str());
        duk_json_decode(ctx_, -1);

        if (duk_pcall(ctx_, 2) != DUK_EXEC_SUCCESS) {
            // Display a stack trace
            duk_get_prop_string(ctx_, -1, "stack");
            std::string err = duk_safe_to_string(ctx_, -1);
            throw std::runtime_error(err);
        } else {
            duk_idx_t resultIdx = -1;
            std::string json = duk_json_encode(ctx_, resultIdx);
            try {
                state = json::parse(json);
            } catch (...) {
                throw std::runtime_error(
                    "Funciton result is not a valid javascript object!");
            }
        }
        duk_pop(ctx_);
    }
    duk_pop(ctx_);
    return state;
}
} // namespace yess::ext
