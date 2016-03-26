#ifndef FUZZY_FILTER_REGEX
#define FUZZY_FILTER_REGEX

#include "./IFilter.h"
#include "../model/types/String.h"

#include <boost/regex.hpp>

class RegexFilter : public IFilter {
private:
   boost::regex _pattern;
   std::string _variable;
public:

   RegexFilter(const std::string variable, std::string pattern) : _pattern(boost::regex(pattern)), _variable(variable) {}

   static bool TestAndCreate(IFilter** filter, std::string str) {
	   boost::smatch matches;
	   const static boost::regex pattern(" *\\$([a-zA-Z0-9]+) *, *(.*) *");

	   if (boost::regex_match(str, matches, pattern)) {
		   *filter = new RegexFilter(matches[1], matches[2]);
		   std::cout << "Got here!" << std::endl;
		   return true;
	   }
	   return false;
   }

   bool Test(const std::vector<VariableSetValue>&& values, std::map<std::string, std::pair<model::types::SubType, unsigned char>>&& variables) override {
	   unsigned char aa = variables[_variable].second;
	   std::string str = std::dynamic_pointer_cast<model::types::String, model::types::Base>(values[aa].dataPointer())->value();
	   return boost::regex_match(str, _pattern);
   }

};


#endif
