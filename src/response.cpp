#include <string>

#include "response.hpp"

namespace yess
{
Response::Response(ResponseStatus status, std::string message)
    : m_status(status), m_message(message)
{
}
std::string Response::getMessage() { return m_message; }
ResponseStatus Response::getStatus() { return m_status; }
bool Response::operator==(const Response &resp) const
{
    return m_message == resp.m_message && m_status == resp.m_status;
}
} // namespace yess
