#pragma once

#include "Poco/JSON/Object.h"
#include <string>
namespace Leves::ext
{
class IContext
{
  public:
    virtual Poco::JSON::Object::Ptr callProjection(const std::string &fnName,
                                                   int arg) = 0;
};
} // namespace Leves::ext
