#include "../debug/duk_trans_socket.h"
#include "../src/ActionHandler.hpp"
#include "../src/ext/DukContext.hpp"
#include "../utils/files.hpp"

#include "Poco/Dynamic/Var.h"
#include "Poco/FileStream.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include "Poco/StreamCopier.h"
#include "duktape.h"
#include "gtest/gtest.h"
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace Leves;

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
    std::vector<Persistance::Entities::Event> events;
    std::string read = readFile("test/projections/proj1.js");

    Leves::ext::DukContext ctx = *m_context.get();
    ctx.read(read);

    std::string initJson = "{}";
    Poco::JSON::Parser parser;
    Poco::Dynamic::Var initState = parser.parse(initJson);

    Poco::JSON::Object::Ptr result =
        ctx.callProjection("projection", events, initState);

    auto resultObj = result.get();
    std::stringstream ss;
    resultObj->stringify(ss);
    std::string resultStr = ss.str();

    ASSERT_EQ(resultStr, initJson);
}

TEST_F(TestESInterop, StreamProjection)
{
    std::vector<Persistance::Entities::Event> events;

    Persistance::Entities::Event e1 = {
        1,             // id
        1,             // streamId
        "UserCreated", // type
        "{"
        "\"username\":\"johndoe\","
        "\"email\":\"johndoe@mail.com\""
        "}", // payload
        0    // version
    };

    Persistance::Entities::Event e2 = {
        2,                  // id
        1,                  // streamId
        "UserEmailChanged", // type
        "{"
        "\"newEmail\":\"johndoe2@mail.com\""
        "}", // payload
        1    // version
    };

    events.push_back(e1);
    // events.push_back(e2);

    std::string read = readFile("test/projections/userproj.js");

    Leves::ext::DukContext ctx = *m_context.get();
    ctx.read(read);

    std::string initJson = "{}";
    Poco::JSON::Parser parser;
    Poco::Dynamic::Var initState = parser.parse(initJson);

    Poco::JSON::Object::Ptr result =
        ctx.callProjection("projection", events, initState);

    auto resultObj = result.get();
    std::stringstream ss;
    resultObj->stringify(ss);
    std::string resultStr = ss.str();
    std::string expected = "{"
                           "\"username\":\"johndoe\","
                           "\"email\":\"johndoe2@mail.com\""
                           "}";

    ASSERT_EQ(resultStr, expected);
}
