#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "duktape.h"
#include "nlohmann/json.hpp"
#include "gtest/gtest-message.h"
#include "gtest/gtest-test-part.h"
#include "gtest/gtest_pred_impl.h"

#include "../debug/duk_trans_socket.h"
#include "../src/action_handler.hpp"
#include "../src/db/entities/event.hpp"
#include "../src/ext/duk_context.hpp"
#include "../src/server.hpp"
#include "../utils/files.hpp"

using json = nlohmann::json;
using namespace yess;

class TestESInterop : public testing::Test
{
  public:
    std::unique_ptr<ext::Duk_context> ctx_;
    std::unique_ptr<Server> server_;
    std::unique_ptr<Action_handler> handler_;

    void SetUp()
    {
        ctx_ = std::make_unique<ext::Duk_context>();

        duk_trans_socket_init();
        duk_trans_socket_waitconn();

        duk_debugger_attach(ctx_.get()->get_duk_ctx(),
                            duk_trans_socket_read_cb,
                            duk_trans_socket_write_cb,
                            duk_trans_socket_peek_cb,
                            duk_trans_socket_read_flush_cb,
                            duk_trans_socket_write_flush_cb,
                            nullptr,
                            nullptr,
                            nullptr);

        server_ = std::make_unique<Server>();
        server_->initDB();
        handler_ = std::make_unique<Action_handler>("yess.db");
    }

    void TearDown()
    {
        duk_debugger_detach(ctx_.get()->get_duk_ctx());
        duk_trans_socket_finish();
    }
};

TEST_F(TestESInterop, EmptyStreamProjection)
{
    std::vector<db::Event> events;
    std::string read = read_file("test/projections/proj1.js");

    ctx_->read(read);

    std::string init_json = "{}";
    json init_state = json::parse(init_json);

    json result = ctx_->call_projection("projection", events, init_state);

    ASSERT_EQ(init_state, result);
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

    std::string read = read_file("test/projections/userproj.js");

    ctx_->read(read);

    json init_state = json({});

    json result = ctx_->call_projection("projection", events, init_state);

    std::string exp_str = "{"
                          "\"username\":\"johndoe\","
                          "\"email\":\"johndoe2@mail.com\""
                          "}";

    json expected = json::parse(exp_str);
    ASSERT_EQ(result, expected);
}
