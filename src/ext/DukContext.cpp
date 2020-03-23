#include "DukContext.hpp"
#include "../Persistance/Entities/Event.hpp"
#include "Poco/Dynamic/Var.h"
#include "Poco/Exception.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include "duk_config.h"
#include "duktape.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace Leves::ext
{
DukContext::DukContext()
{
    //
    m_pCtx = duk_create_heap_default();
}

DukContext::~DukContext()
{
    //
    duk_destroy_heap(m_pCtx);
}

void DukContext::read(const std::string &body)
{
    duk_push_string(m_pCtx, body.c_str());
    if (duk_peval(m_pCtx) != 0) {
        duk_get_prop_string(m_pCtx, -1, "stack");
        throw std::runtime_error(duk_safe_to_string(m_pCtx, -1));
    }
    duk_pop(m_pCtx); // ignore result
    duk_push_global_object(m_pCtx);
} // namespace Leves::ext

Poco::JSON::Object::Ptr DukContext::callProjection(
    const std::string &fnName,
    const std::vector<Persistance::Entities::Event> &events,
    Poco::Dynamic::Var initState)
{
    Poco::Dynamic::Var state = initState;
    Poco::JSON::Object::Ptr result;
    if (events.size() == 0) {
        std::cout << "No events" << std::endl;
        result = state.extract<Poco::JSON::Object::Ptr>();
        return result;
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
    Poco::JSON::Parser parser;
    for (const auto &event : events) {
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

        duk_idx_t stIdx = duk_push_object(m_pCtx);
        std::string jsonState = parser.parse(state);
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
            Poco::Dynamic::Var state;
            try {
                state = parser.parse(json);
            } catch (Poco::Exception &exc) {
                // throw std::runtime_error(exc.displayText());
                throw std::runtime_error(
                    "Funciton result is not a valid javascript object!");
            }
        }
        // duk_pop(m_pCtx);
    }
    duk_pop(m_pCtx);
    result = state.extract<Poco::JSON::Object::Ptr>();
    return result;
}
} // namespace Leves::ext
