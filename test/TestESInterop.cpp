#include "../src/ActionHandler.hpp"
#include "../src/ext/DukContext.hpp"
#include "../utils/files.hpp"

#include "Poco/Dynamic/Var.h"
#include "Poco/FileStream.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include "Poco/StreamCopier.h"
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
    std::unique_ptr<ext::IContext> m_context;
    std::unique_ptr<Server> m_server;
    std::unique_ptr<ActionHandler> m_handler;

    void SetUp()
    {
        m_context = std::make_unique<ext::DukContext>();
        m_server = std::make_unique<Server>();
        m_handler = std::make_unique<ActionHandler>();
    }

    void TearDown() {}
};

TEST_F(TestESInterop, EmptyStreamProjection)
{
    std::vector<Persistance::Entities::Event> events;
    std::string read = readFile("test/projections/proj1.js");

    Leves::ext::DukContext ctx;
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
