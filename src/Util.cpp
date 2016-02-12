#include "Util.h"
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <locale>
#include <cctype>

namespace util
{
    // See http://stackoverflow.com/questions/236129/split-a-string-in-c
    std::vector<std::string>& _splitPrivate(const std::string &s, char delim, std::vector<std::string> &elems)
    {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim))
        {
            elems.push_back(item);
        }
        return elems;
    }

    std::vector<std::string> split(const std::string &s, char delim)
    {
        std::vector<std::string> elems;
        _splitPrivate(s, delim, elems);
        return elems;
    }

		std::string toUppercase(const std::string &str)
		{
			std::string uppercase(str.c_str());
			std::transform(uppercase.begin(), uppercase.end(), uppercase.begin(), [](unsigned char c) { return std::toupper(c, std::locale("en_US.utf8")); });
			return uppercase;
		}

		std::string toLowercase(const std::string &str)
		{
			std::string lowercase(str.c_str());
			std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), [](unsigned char c) { return std::tolower(c, std::locale("en_US.utf8")); });
			return lowercase;
		}

}
