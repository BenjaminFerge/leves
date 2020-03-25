#pragma once

#include <cassert>
#include <string>

#include "Poco/FileStream.h"
#include "Poco/StreamCopier.h"

std::string readFile(const std::string &path);
