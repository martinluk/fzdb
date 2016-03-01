#ifndef FUZZY_IFILTER
#define FUZZY_IFILTER

#include <string>
#include <vector>
#include <memory>
#include "../model/types/Base.h"
#include "../VariableSet.h"

class IFilter {
public:
   virtual bool Test(const std::vector<VariableSetValue>&& values, 
	   std::map<std::string, std::pair<model::types::Base::Subtype, unsigned char>>&& variables) = 0;

   virtual ~IFilter() {}
};

#endif
