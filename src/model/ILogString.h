#ifndef MODEL_ILOGSTRING_H
#define MODEL_ILOGSTRING_H

#include <string>

class Database;

// Interface implemented by classes which need a string in order
// to be printed to a debug log.
class ILogString
{
public:
    virtual ~ILogString() {}

    virtual std::string logString(const Database* db = NULL) const = 0;
};

#endif  // MODEL_ILOGSTRING_H
