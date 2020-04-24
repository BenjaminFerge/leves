#ifndef YESS_PROJECTION_HPP
#define YESS_PROJECTION_HPP

#include <string>

namespace yess::db
{
struct Projection {
    int id;
    std::string type;
    std::string data;
};
} // namespace yess::db

#endif // YESS_PROJECTION_HPP
