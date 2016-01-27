#ifndef FUZZY_FILTER_REGEX
#define FUZZY_FILTER_REGEX

#include "./IFilter.h"

#include <boost/regex.hpp>

class RegexFilter : public IFilter {
private:
   boost::regex _pattern;
   std::string _variable;
public:

   RegexFilter(const std::string variable, std::string pattern) : _pattern(boost::regex(_pattern)), _variable(variable) {}

   bool Test(const std::vector<std::string>&& values, std::map<std::string, std::pair<model::types::Base::Subtype, unsigned char>>&& variables) override {
	   unsigned char aa = variables[_variable].second;
	   return boost::regex_match(values[aa], _pattern);
   }

};


#endif
