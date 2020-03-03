#pragma once

#include "Poco/JSON/Object.h"
#include <string>

namespace Leves
{
enum ResponseStatus { OK, Error };
class Response
{
  public:
    Response(ResponseStatus status, std::string message);
    std::string getMessage();
    ResponseStatus getStatus();

  private:
    ResponseStatus m_status;
    std::string m_message;
};
} // namespace Leves
