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
    Sqlite_repository(std::string conn_key, std::string conn_str);
    virtual ~Sqlite_repository();
    void init_db();

  protected:
    std::unique_ptr<SQLite::Database> db_;
    std::string conn_key_;
    std::string conn_str_;
};
} // namespace yess::db

#endif // YESS_SQLITE_REPOSITORY_HPP
