#pragma once

#include <string>
#include <vector>

#include "../db/entities/event.hpp"
#include "context.hpp"
#include "duktape.h"
#include "nlohmann/json_fwd.hpp"

using json = nlohmann::json;

namespace yess::ext
{
class Duk_context : public Context
{
  public:
    Duk_context();
    ~Duk_context();
    json call_projection(const std::string &fn_name,
                         const std::vector<db::Event> &events,
                         json init_state) override;
    void read(const std::string &body);
    duk_context *get_duk_ctx();

  private:
    duk_context *ctx_;
};
} // namespace yess::ext
