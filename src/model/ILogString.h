#ifndef MODEL_ILOGSTRING_H
#define MODEL_ILOGSTRING_H

#include <string>

class Database;

class ILogString
{
public:
    virtual ~ILogString() {}

    virtual std::string logString(const Database* db = NULL) const = 0;
};

#endif  // MODEL_ILOGSTRING_H
