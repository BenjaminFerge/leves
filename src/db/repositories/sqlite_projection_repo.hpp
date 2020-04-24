#ifndef YESS_SQLITE_PROJECTION_REPO_HPP
#define YESS_SQLITE_PROJECTION_REPO_HPP

#include "projection_repo.hpp"
#include <SQLiteCpp/Database.h>
#include <memory>

namespace yess::db
{
class Sqlite_projection_repo : public Projection_repo, public Sqlite_repository
{
  public:
    Sqlite_projection_repo(std::string conn_key, std::string conn_str);
    void create(Projection p) override;
    std::vector<Projection> all() override;
    Projection id(int id) override;
    std::vector<Projection> type(std::string t) override;
    Projection remove(int id) override;
};
} // namespace yess::db

#endif // YESS_SQLITE_PROJECTION_REPO_HPP
