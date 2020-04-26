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
    Sqlite_projection_repo(std::shared_ptr<SQLite::Database> db);
    void create(Projection p) override;
    std::vector<Projection> all() override;
    Projection id(int id) override;
    std::vector<Projection> type(std::string t) override;
    void remove(int id) override;
};
} // namespace yess::db

#endif // YESS_SQLITE_PROJECTION_REPO_HPP
