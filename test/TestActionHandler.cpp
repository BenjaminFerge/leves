#include <iostream>
#include <memory>
#include <string>

#include "../src/ActionHandler.hpp"
#include "../src/Response.hpp"
#include "../src/Server.hpp"
#include "../utils/files.hpp"
#include "gtest/gtest-message.h"
#include "gtest/gtest-test-part.h"
#include "gtest/gtest_pred_impl.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace yess;

class ActionHandlerTest : public testing::Test
{
  public:
    std::unique_ptr<Server> m_server;
    std::unique_ptr<ActionHandler> m_handler;

    void SetUp()
    {
        m_server = std::make_unique<Server>();
        m_server->initDB();
        m_handler = std::make_unique<ActionHandler>();
    }

    void TearDown()
    {
        // TODO: cleanup database
    }
};

TEST_F(ActionHandlerTest, CreateStream)
{
    auto createStreamJson = readFile("../test/commands/CreateStream.json");
    std::cout << createStreamJson << std::endl;
    json obj = json::parse(createStreamJson);
    Response resp = m_handler->handle(obj);
    Response expResp(ResponseStatus::OK, "{\"status\":\"OK\"}");
    ASSERT_EQ(resp, expResp);
}
