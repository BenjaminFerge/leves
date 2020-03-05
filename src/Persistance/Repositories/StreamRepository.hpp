#pragma once

#include <memory>
#include <vector>

#include "../Entities/Stream.hpp"
#include "Poco/Data/Session.h"

namespace Poco {
namespace Data {
class Session;
}  // namespace Data
}  // namespace Poco

namespace Leves::Persistance::Repositories
{
class StreamRepository
{
  public:
    StreamRepository();
    ~StreamRepository();
    std::vector<Entities::Stream> all();
    Entities::Stream get(int id);
    void create(Entities::Stream stream);
    void initDB();

  private:
    std::unique_ptr<Poco::Data::Session> m_session;
};
} // namespace Leves::Persistance::Repositories
