#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../debug/duk_trans_socket.h"
#include "../src/ActionHandler.hpp"
#include "../src/Response.hpp"
#include "../src/Server.hpp"
#include "../src/db/Entities/Event.hpp"
#include "../src/ext/DukContext.hpp"
#include "../utils/files.hpp"
#include "duktape.h"
#include "nlohmann/json.hpp"
#include "gtest/gtest-message.h"
#include "gtest/gtest-test-part.h"
#include "gtest/gtest_pred_impl.h"

using json = nlohmann::json;
using namespace yess;

class TestESInterop : public testing::Test
{
  public:
    std::unique_ptr<ext::DukContext> m_context;
    std::unique_ptr<Server> m_server;
    std::unique_ptr<ActionHandler> m_handler;

    void SetUp()
    {
        m_context = std::make_unique<ext::DukContext>();

        duk_trans_socket_init();
        duk_trans_socket_waitconn();

        duk_debugger_attach(m_context.get()->getDukContext(),
                            duk_trans_socket_read_cb,
                            duk_trans_socket_write_cb,
                            duk_trans_socket_peek_cb,
                            duk_trans_socket_read_flush_cb,
                            duk_trans_socket_write_flush_cb,
                            nullptr,
                            nullptr,
                            nullptr);

        m_server = std::make_unique<Server>();
        m_server->initDB();
        m_handler = std::make_unique<ActionHandler>();
    }

    void TearDown()
    {
        duk_debugger_detach(m_context.get()->getDukContext());
        duk_trans_socket_finish();
    }
};

TEST_F(TestESInterop, EmptyStreamProjection)
{
    std::vector<db::Event> events;
    std::string read = readFile("test/projections/proj1.js");

    m_context->read(read);

    std::string initJson = "{}";
    json initState = json::parse(initJson);

    json result = m_context->callProjection("projection", events, initState);

    ASSERT_EQ(initState, result);
}

TEST_F(TestESInterop, StreamProjection)
{
    std::vector<db::Event> events;

    db::Event e1 = {
        1,             // id
        1,             // streamId
        "UserCreated", // type
        "{"
        "\"username\":\"johndoe\","
        "\"email\":\"johndoe@mail.com\""
        "}", // payload
        0    // version
    };

    db::Event e2 = {
        2,                  // id
        1,                  // streamId
        "UserEmailChanged", // type
        "{"
        "\"newEmail\":\"johndoe2@mail.com\""
        "}", // payload
        1    // version
    };

    events.push_back(e1);
    events.push_back(e2);

    std::string read = readFile("test/projections/userproj.js");

    m_context->read(read);

    json initState = json({});

    json result = m_context->callProjection("projection", events, initState);

    std::string expectedStr = "{"
                              "\"username\":\"johndoe\","
                              "\"email\":\"johndoe2@mail.com\""
                              "}";

    json expected = json::parse(expectedStr);
    ASSERT_EQ(result, expected);
}
