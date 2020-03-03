#include <string>

#include "Response.hpp"

namespace Leves
{
Response::Response(ResponseStatus status, std::string message)
    : m_status(status), m_message(message)
{
}
std::string Response::getMessage()
{
    std::string resp;
    switch (m_status) {
    case ResponseStatus::OK:
        resp += "[OK] ";
        break;
    case ResponseStatus::Error:
        resp += "[Error] ";
        break;
    }
    resp += m_message + "\n";
    return resp;
}
ResponseStatus Response::getStatus() { return m_status; }
} // namespace Leves
