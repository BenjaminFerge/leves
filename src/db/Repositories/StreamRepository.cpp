#include <bits/exception.h>
#include <filesystem>
#include <string>

#include "../../log.hpp"
#include "StreamRepository.hpp"

namespace fs = std::filesystem;

namespace yess::db
{
StreamRepository::StreamRepository(std::string connectorKey,
                                   std::string connectionString)
    : m_connectorKey(connectorKey), m_connectionString(connectionString)
{
}

void StreamRepository::initDB()
{
    log::info("Connection: {}, {}", m_connectorKey, m_connectionString);
}

StreamRepository::~StreamRepository() {}
} // namespace yess::db
