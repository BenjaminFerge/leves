#pragma once

#include <string>

namespace yess
{
std::string readFile(const std::string &path);
std::string path_abs(const std::string &filename);
} // namespace yess
