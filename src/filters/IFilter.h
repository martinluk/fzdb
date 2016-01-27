#ifndef FUZZY_IFILTER
#define FUZZY_IFILTER

#include <string>
#include <vector>
#include "../model/types/Base.h"

class IFilter {
public:
   virtual bool Test(const std::vector<std::string>&& values, std::map<std::string, std::pair<model::types::Base::Subtype, unsigned char>>&& variables) = 0;
};

#endif
