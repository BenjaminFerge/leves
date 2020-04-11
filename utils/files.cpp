#include <bits/exception.h>
#include <cassert>
#include <filesystem>
#include <string>

#include "Poco/FileStream.h"
#include "Poco/StreamCopier.h"

#include "../src/log.hpp"
#include "./files.hpp"

namespace fs = std::filesystem;

namespace yess
{
std::string read_file(const std::string &path)
{
    Poco::FileInputStream fis(path);
    assert(fis.good());
    std::string read;
    Poco::StreamCopier::copyToString(fis, read);
    assert(!read.empty());
    return read;
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
