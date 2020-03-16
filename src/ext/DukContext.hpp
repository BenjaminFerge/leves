#pragma once

#include "IContext.hpp"
#include "Poco/JSON/Object.h"
#include "duktape.h"
#include <string>

namespace Leves::ext
{
class DukContext : public IContext
{
  public:
    DukContext();
    ~DukContext();
    virtual Poco::JSON::Object::Ptr callProjection(const std::string &fnName,
                                                   int arg);
    void read(const std::string &body);

  private:
    duk_context *m_pCtx;
};
} // namespace Leves::ext
