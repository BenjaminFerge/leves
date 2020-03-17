#include "../src/ActionHandler.hpp"
#include "../src/ext/DukContext.hpp"
#include "Poco/Dynamic/Var.h"
#include "Poco/FileStream.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include "Poco/StreamCopier.h"
#include "gtest/gtest.h"
#include <memory>

using namespace Leves;

class TestESInterop : public testing::Test
{
  public:
    std::unique_ptr<ext::IContext> m_context;

    void SetUp() { m_context = std::make_unique<ext::DukContext>(); }

    void TearDown() {}
};

TEST_F(TestESInterop, CreateStream)
{
    Poco::FileInputStream fis("test/projections/proj1.js");
    assert(fis.good());
    std::string read;
    Poco::StreamCopier::copyToString(fis, read);
    assert(!read.empty());

    Leves::ext::DukContext ctx;
    ctx.read(read);
    Poco::JSON::Object::Ptr result = ctx.callProjection("projection", 100);
    int num = result->getValue<int>("num");
    std::string str = result->getValue<std::string>("str");

    ASSERT_EQ(num, 110);
    ASSERT_EQ(str, "xyz-20");
}
