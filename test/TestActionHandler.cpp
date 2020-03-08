#include "../src/ActionHandler.hpp"
#include "Poco/Dynamic/Var.h"
#include "Poco/FileStream.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include "gtest/gtest.h"
#include <cassert>
#include <iostream>

std::string readFile(const std::string &path)
{
    Poco::FileInputStream fis(path);
    assert(fis.good());
    std::string read;
    while (!fis.eof()) {
        read += fis.get();
    }
    assert(!read.empty());
    return read;
}

void testActionHandler()
{
    Leves::Server server;
    Leves::ActionHandler handler;
    auto createStreamJson = readFile("../test/commands/CreateStream.json");
    std::cout << createStreamJson << std::endl;
    Poco::JSON::Parser parser;
    Poco::Dynamic::Var parsed = parser.parse(createStreamJson);
    Poco::JSON::Object::Ptr cmdObj = parsed.extract<Poco::JSON::Object::Ptr>();
    handler.handle(cmdObj);
}

TEST(ActionHandlerTest, ReadFile)
{
    testActionHandler();
    EXPECT_TRUE(true);
}
