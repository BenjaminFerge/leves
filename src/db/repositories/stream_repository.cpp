#include <bits/exception.h>
#include <filesystem>
#include <string>

#include "../../log.hpp"
#include "stream_repository.hpp"

namespace fs = std::filesystem;

namespace yess::db
{
Stream_repository::Stream_repository(std::string connectorKey,
                                     std::string connectionString)
    : m_connectorKey(connectorKey), m_connectionString(connectionString)
{
}

void Stream_repository::initDB()
{
    log::info("Connection: {}, {}", m_connectorKey, m_connectionString);
}

Stream_repository::~Stream_repository() {}
} // namespace yess::db
