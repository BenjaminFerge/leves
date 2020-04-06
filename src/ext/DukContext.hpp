#pragma once

#include <string>
#include <vector>

#include "../db/Entities/Event.hpp"
#include "IContext.hpp"
#include "duktape.h"
#include "nlohmann/json_fwd.hpp"

using json = nlohmann::json;

namespace yess::ext
{
class DukContext : public IContext
{
  public:
    DukContext();
    ~DukContext();
    json callProjection(const std::string &fnName,
                        const std::vector<db::Event> &events,
                        json initState) override;
    void read(const std::string &body);
    duk_context *getDukContext();

  private:
    duk_context *m_pCtx;
};
} // namespace yess::ext
