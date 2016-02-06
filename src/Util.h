#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <string>

namespace util
{
    std::vector<std::string> split(const std::string &s, char delim);
		std::string toUppercase(const std::string &str);
		std::String toLowercase(const std::string &str);
}

#endif  // UTIL_H
