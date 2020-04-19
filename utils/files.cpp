#include <bits/exception.h>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>

#include "../src/log.hpp"
#include "./files.hpp"

namespace fs = std::filesystem;

namespace yess
{
std::string read_file(const std::string &path)
{
    std::ifstream ifs(path);
    std::stringstream buffer;
    buffer << ifs.rdbuf();
    return buffer.str();
}

std::string path_to_abs(const std::string &f)
{
    std::string filename = f;
    fs::path path(filename);
    if (path.is_relative()) {
        filename = fs::absolute(filename);
        log::info("The given relative path is converted to an absolute: '{}'",
                  filename);
    }
    return filename;
}
} // namespace yess
