#pragma once

#include "../db/Entities/Event.hpp"
#include "IContext.hpp"
#include "Poco/Dynamic/Var.h"
#include "Poco/JSON/Object.h"
#include "duktape.h"
#include <string>

namespace leves::ext
{
class DukContext : public IContext
{
  public:
    DukContext();
    ~DukContext();
    virtual Poco::JSON::Object::Ptr
    callProjection(const std::string &fnName,
                   const std::vector<db::Event> &events,
                   Poco::Dynamic::Var initState);
    void read(const std::string &body);
    duk_context *getDukContext();

  private:
    duk_context *m_pCtx;
};
} // namespace leves::ext
