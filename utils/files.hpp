#pragma once

#include "Poco/FileStream.h"
#include "Poco/StreamCopier.h"
#include <cassert>
#include <string>

std::string readFile(const std::string &path);
