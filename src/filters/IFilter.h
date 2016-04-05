#ifndef FUZZY_IFILTER
#define FUZZY_IFILTER

#include <string>
#include <vector>
#include <memory>
#include "../types/Base.h"
#include "../VariableSet.h"

class IFilter {
public:
   virtual bool Test(const VariableSet&& variableSet, const std::vector<VariableSetValue>&& values) = 0;

   virtual ~IFilter() {}
};

#endif
