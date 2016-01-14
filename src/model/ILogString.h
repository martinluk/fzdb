#ifndef MODEL_ILOGSTRING_H
#define MODEL_ILOGSTRING_H

#include <string>

class ILogString
{
public:
    virtual ~ILogString() {}

    virtual std::string logString() const = 0;
};

#endif  // MODEL_ILOGSTRING_H
