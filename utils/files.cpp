#include "files.hpp"
#include "../src/log.hpp"

#include "Poco/FileStream.h"
#include "Poco/StreamCopier.h"
#include <cassert>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

namespace yess
{
std::string readFile(const std::string &path)
{
    Poco::FileInputStream fis(path);
    assert(fis.good());
    std::string read;
    Poco::StreamCopier::copyToString(fis, read);
    assert(!read.empty());
    return read;
}

std::string path_abs(const std::string &f)
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
