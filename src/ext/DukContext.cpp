#include "DukContext.hpp"
#include "Poco/Dynamic/Var.h"
#include "Poco/Exception.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include "duk_config.h"
#include "duktape.h"
#include <iostream>
#include <stdexcept>
#include <string>

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
    /*
duk_push_string(m_pCtx, body.c_str());
if (duk_peval(m_pCtx) != 0) {
    duk_get_prop_string(m_pCtx, -1, "stack");
    throw std::runtime_error(duk_safe_to_string(m_pCtx, -1));
}
duk_pop(m_pCtx); // ignore result
duk_push_global_object(m_pCtx);
*/

    // Compile the JS into bytecode
    if (duk_pcompile_string(m_pCtx, 0, body.c_str()) != 0) {
        auto err = duk_safe_to_string(m_pCtx, -1);
        duk_pop(m_pCtx);
        throw std::runtime_error(err);
    }
    // Actually evaluate it - this will push the compiled code into the
    // global scope
    duk_pcall(m_pCtx, 0);
    duk_pop(m_pCtx);
} // namespace Leves::ext

Poco::JSON::Object::Ptr DukContext::callProjection(const std::string &fnName,
                                                   int arg)
{
    duk_idx_t objIdx = 0;
    duk_idx_t fnIdx = 0;

    if (!duk_get_global_string(m_pCtx, fnName.c_str())) {
        throw std::runtime_error("Function '" + fnName + "' is not found!");
    }
    /*
    if (!duk_is_valid_index(m_pCtx, objIdx)) {
        throw std::runtime_error("Invalid index!");
    }

    if (duk_get_prop_string(m_pCtx, objIdx, fnName.c_str()) == 0) {
        throw std::runtime_error("Function '" + fnName + "' is not found!");
    }
    duk_push_int(m_pCtx, 10);
    // duk_push_int(m_pCtx, 20);
    if (duk_get_prop_string(m_pCtx, objIdx, fnName.c_str()) == 0) {
        throw std::runtime_error("Function '" + fnName + "' is not found!");
    }
    */
    /*
    if (duk_is_function(m_pCtx, fnIdx) == 0) {
        throw std::runtime_error("Defined '" + fnName +
                                 "' object is not a function!");
    }
    if (duk_is_callable(m_pCtx, fnIdx) == 0) {
        throw std::runtime_error("Defined '" + fnName +
                                 "' object is not callable!");
    }
    */
    // Fill parameter
    // duk_push_int(m_pCtx, 30);
    /*
    if (duk_get_prop_string(m_pCtx, objIdx, fnName.c_str()) == 0) {
        throw std::runtime_error("Function '" + fnName + "' is not found!");
    }
    */
    duk_push_int(m_pCtx, 10);
    duk_push_int(m_pCtx, 20);

    Poco::JSON::Object::Ptr result;
    if (duk_pcall(m_pCtx, 3 != DUK_EXEC_SUCCESS)) {
        // Display a stack trace
        duk_get_prop_string(m_pCtx, -1, "stack");
        throw std::runtime_error(duk_safe_to_string(m_pCtx, -1));
    } else {
        // std::cout << "Result: " << duk_safe_to_string(m_pCtx, -1) <<
        // std::endl;
        duk_idx_t resultIdx = -1;
        std::string json = duk_json_encode(m_pCtx, resultIdx);
        std::cout << "JSON: " << json << std::endl;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var var;
        try {
            var = parser.parse(json);
        } catch (Poco::Exception &exc) {
            // throw std::runtime_error(exc.displayText());
            throw std::runtime_error(
                "Funciton result is not a valid javascript object!");
        }
        result = var.extract<Poco::JSON::Object::Ptr>();
    }
    duk_pop(m_pCtx);
    return result;
}
} // namespace Leves::ext
