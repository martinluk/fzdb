#ifndef FUZZY_IFILTER
#define FUZZY_IFILTER

#include <string>
#include <vector>
#include <memory>
#include "../types/base.h"
#include "../variable_set.h"

class IFilter {
 public:
  virtual bool Test(const VariableSet&& variableSet, const VariableSetRow&& values) = 0;

  virtual ~IFilter() {}
};

#endif
