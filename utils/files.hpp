#pragma once

#include <string>

namespace yess
{
std::string read_file(const std::string& path);
std::string path_to_abs(const std::string& filename);
} // namespace yess
