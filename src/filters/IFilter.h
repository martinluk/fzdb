#ifndef FUZZY_IFILTER
#define FUZZY_IFILTER

#include <string>
#include <vector>

class IFilter {
public:
   virtual bool Test(const std::vector<std::string>&& values) = 0;
};

#endif
