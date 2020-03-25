#include "../src/ActionHandler.hpp"
#include "../utils/files.hpp"

#include "Poco/Dynamic/Var.h"
#include "Poco/FileStream.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include "Poco/StreamCopier.h"
#include "gtest/gtest.h"
#include <iostream>
#include <memory>

using namespace leves;

class ActionHandlerTest : public testing::Test
{
  public:
    std::unique_ptr<Server> m_server;
    std::unique_ptr<ActionHandler> m_handler;

    void SetUp()
    {
        m_server = std::make_unique<Server>();
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
    Poco::JSON::Parser parser;
    Poco::Dynamic::Var parsed = parser.parse(createStreamJson);
    Poco::JSON::Object::Ptr cmdObj = parsed.extract<Poco::JSON::Object::Ptr>();
    auto resp = m_handler->handle(cmdObj);
    Response expResp(ResponseStatus::OK, "{\"status\":\"OK\"}");
    ASSERT_EQ(resp, expResp);
}
