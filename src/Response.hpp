#pragma once

#include <string>

#include "Poco/JSON/Object.h"

namespace leves
{
enum ResponseStatus { OK, Error };
class Response
{
  public:
    Response(ResponseStatus status, std::string message);
    std::string getMessage();
    ResponseStatus getStatus();
    bool operator==(const Response &resp) const;

  private:
    ResponseStatus m_status;
    std::string m_message;
};
} // namespace leves
