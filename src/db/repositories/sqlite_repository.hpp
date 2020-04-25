#ifndef YESS_SQLITE_REPOSITORY_HPP
#define YESS_SQLITE_REPOSITORY_HPP

#include <SQLiteCpp/Database.h>
#include <memory>
#include <string>
namespace yess::db
{
class Sqlite_repository
{
  public:
    explicit Sqlite_repository(std::shared_ptr<SQLite::Database> db);
    virtual ~Sqlite_repository();
    static std::shared_ptr<SQLite::Database> init_db(std::string conn_str);

  protected:
    std::shared_ptr<SQLite::Database> db_;
};
} // namespace yess::db

#endif // YESS_SQLITE_REPOSITORY_HPP
