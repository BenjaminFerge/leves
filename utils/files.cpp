#include "files.hpp"

#include "Poco/FileStream.h"
#include "Poco/StreamCopier.h"
#include <cassert>
#include <string>

std::string readFile(const std::string &path)
{
    Poco::FileInputStream fis(path);
    assert(fis.good());
    std::string read;
    Poco::StreamCopier::copyToString(fis, read);
    assert(!read.empty());
    return read;
}

