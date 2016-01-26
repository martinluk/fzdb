#ifndef FUZZY_FILTER_REGEX
#define FUZZY_FILTER_REGEX

#include "./IFilter.h"

#include <boost/regex.hpp>

class RegexFilter : public IFilter {
private:
   boost::regex _pattern;
public:

   RegexFilter(std::string pattern) : _pattern(boost::regex(_pattern)) {}

   bool Test(const std::vector<std::string>&& values) override {
      return boost::regex_match(values[0], _pattern);
   }

};


#endif
