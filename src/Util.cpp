#include "Util.h"
#include <sstream>

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
}
