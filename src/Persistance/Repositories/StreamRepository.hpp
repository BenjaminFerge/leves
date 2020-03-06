#pragma once

#include <string>
#include <vector>

#include "../Entities/Stream.hpp"
#include "Poco/Data/Session.h"

namespace Poco
{
namespace Data
{
class Session;
} // namespace Data
} // namespace Poco

namespace Leves::Persistance::Repositories
{
class StreamRepository
{
  public:
    StreamRepository(std::string connectorKey, std::string connectionString);
    ~StreamRepository();
    std::vector<Entities::Stream> all();
    Entities::Stream get(int id);
    void create(Entities::Stream stream);
    void initDB();
    Poco::Data::Session makeSession();

  private:
    std::string m_connectorKey;
    std::string m_connetctionString;
};
} // namespace Leves::Persistance::Repositories
