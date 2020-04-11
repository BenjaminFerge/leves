#include <iostream>
#include <memory>
#include <string>

#include "gtest/gtest-message.h"
#include "gtest/gtest-test-part.h"
#include "gtest/gtest_pred_impl.h"
#include <nlohmann/json.hpp>

#include "../src/action_handler.hpp"
#include "../src/response.hpp"
#include "../src/server.hpp"
#include "../utils/files.hpp"

using json = nlohmann::json;
using namespace yess;

class Action_handler_test : public testing::Test
{
  public:
    std::unique_ptr<Server> m_server;
    std::unique_ptr<Action_handler> m_handler;

    void SetUp()
    {
        m_server = std::make_unique<Server>();
        m_server->initDB();
        m_handler = std::make_unique<Action_handler>();
    }

    void TearDown()
    {
        // TODO: cleanup database
    }
};

TEST_F(Action_handler_test, CreateStream)
{
    auto create_stream_json = read_file("../test/commands/CreateStream.json");
    std::cout << create_stream_json << std::endl;
    json obj = json::parse(create_stream_json);
    Response resp = m_handler->handle(obj);
    Response exp_resp(ResponseStatus::OK, "{\"status\":\"OK\"}");
    ASSERT_EQ(resp, exp_resp);
}
