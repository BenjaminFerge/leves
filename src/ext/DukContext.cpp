#include <stdexcept>
#include <string>
#include <vector>

#include "../db/Entities/Event.hpp"
#include "../log.hpp"
#include "DukContext.hpp"
#include "duktape.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace yess::ext
{
DukContext::DukContext()
{
    //
    m_pCtx = duk_create_heap_default();
}

DukContext::~DukContext()
{
    // FIXME: causes segfault
    // duk_destroy_heap(m_pCtx);
}

duk_context *DukContext::getDukContext() { return m_pCtx; }

void DukContext::read(const std::string &body)
{
    duk_push_string(m_pCtx, body.c_str());
    if (duk_peval(m_pCtx) != 0) {
        duk_get_prop_string(m_pCtx, -1, "stack");
        throw std::runtime_error(duk_safe_to_string(m_pCtx, -1));
    }
    duk_pop(m_pCtx); // ignore result
    duk_push_global_object(m_pCtx);
}

json DukContext::callProjection(const std::string &fnName,
                                const std::vector<db::Event> &events,
                                json initState)
{
    json state = initState;
    if (events.size() == 0) {
        yess::log::warn("The called projection has no events");
        return state;
    }
    duk_idx_t objIdx = 0;
    duk_idx_t fnIdx = 0;

    if (!duk_is_valid_index(m_pCtx, objIdx)) {
        throw std::runtime_error("Invalid index!");
    }
    if (duk_get_prop_string(m_pCtx, objIdx, fnName.c_str()) == 0) {
        throw std::runtime_error("Function '" + fnName + "' is not found!");
    }
    if (duk_get_prop_string(m_pCtx, objIdx, fnName.c_str()) == 0) {
        throw std::runtime_error("Function '" + fnName + "' is not found!");
    }

    // Fill parameters
    for (const auto &event : events) {
        // param 1: event
        duk_idx_t evIdx = duk_push_object(m_pCtx);

        duk_push_int(m_pCtx, event.id);
        duk_put_prop_string(m_pCtx, evIdx, "id");

        duk_push_string(m_pCtx, event.payload.c_str());
        duk_json_decode(m_pCtx, -1);
        duk_put_prop_string(m_pCtx, evIdx, "payload");

        duk_push_int(m_pCtx, event.version);
        duk_put_prop_string(m_pCtx, evIdx, "version");

        duk_push_int(m_pCtx, event.streamId);
        duk_put_prop_string(m_pCtx, evIdx, "streamId");

        duk_push_string(m_pCtx, event.type.c_str());
        duk_put_prop_string(m_pCtx, evIdx, "type");

        // param 2: state
        std::string jsonState = state.dump();
        duk_push_string(m_pCtx, jsonState.c_str());
        duk_json_decode(m_pCtx, -1);

        if (duk_pcall(m_pCtx, 2) != DUK_EXEC_SUCCESS) {
            // Display a stack trace
            duk_get_prop_string(m_pCtx, -1, "stack");
            std::string err = duk_safe_to_string(m_pCtx, -1);
            throw std::runtime_error(err);
        } else {
            duk_idx_t resultIdx = -1;
            std::string json = duk_json_encode(m_pCtx, resultIdx);
            try {
                state = json::parse(json);
            } catch (...) {
                throw std::runtime_error(
                    "Funciton result is not a valid javascript object!");
            }
        }
        duk_pop(m_pCtx);
    }
    duk_pop(m_pCtx);
    return state;
}
} // namespace yess::ext
