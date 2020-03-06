#pragma once

#include <string>
#include <vector>

#include "../Entities/Event.hpp"
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
    void attachEvent(Entities::Event event);
    std::vector<Entities::Event> getEvents(int streamId);
    std::vector<Entities::Event> getEvents(std::string streamType);
    Poco::Data::Session makeSession();

  private:
    std::string m_connectorKey;
    std::string m_connetctionString;
};
} // namespace Leves::Persistance::Repositories
