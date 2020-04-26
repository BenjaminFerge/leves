#ifndef YESS_PROJECTION_REPO_HPP
#define YESS_PROJECTION_REPO_HPP

#include "../entities/projection.hpp"
#include "sqlite_repository.hpp"
#include <vector>

namespace yess::db
{
class Projection_repo
{
  public:
    Projection_repo();
    virtual ~Projection_repo();
    virtual void create(Projection p) = 0;
    virtual std::vector<Projection> all() = 0;
    virtual Projection id(int id) = 0;
    virtual std::vector<Projection> type(std::string t) = 0;
    // TODO: throw error on non-existing id
    virtual void remove(int id) = 0;
};
} // namespace yess::db

#endif // YESS_PROJECTION_REPO_HPP
