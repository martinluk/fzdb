#ifndef FUZZY_FILTER_LEV
#define FUZZY_FILTER_LEV

#include <numeric>

#include "./ifilter.h"
#include "../types/string.h"

#include <boost/regex.hpp>

unsigned int levenshtein_distance(const std::string &s1, const std::string &s2)
{
  // To change the type this function manipulates and returns, change
  // the return type and the types of the two variables below.
  unsigned int s1len = s1.size();
  unsigned int s2len = s2.size();
  
  auto column_start = (decltype(s1len))1;
  
  auto column = new decltype(s1len)[s1len + 1];
  std::iota(column + column_start, column + s1len + 1, column_start);
  
  for (auto x = column_start; x <= s2len; x++) {
    column[0] = x;
    auto last_diagonal = x - column_start;
    for (auto y = column_start; y <= s1len; y++) {
      auto old_diagonal = column[y];
      auto possibilities = {
        column[y] + 1,
        column[y - 1] + 1,
        last_diagonal + (s1[y - 1] == s2[x - 1]? 0 : 1)
      };
      column[y] = std::min(possibilities);
      last_diagonal = old_diagonal;
    }
  }
  auto result = column[s1len];
  delete[] column;
  return result;
}

class LevFilter : public IFilter {
private:
   const uint32_t _maxdist;
   const std::string _target;
   const std::string _variable;
public:

   LevFilter(const std::string variable, std::string target, uint32_t maxdist) 
    : _target(target), _variable(variable),  _maxdist(maxdist) { }

   static bool TestAndCreate(IFilter** filter, std::string str) {
       boost::smatch matches;
       const static boost::regex pattern(" *\\$([a-zA-Z0-9]+) *, *\\\"([^\"]*)\\\" *, *([0-9]+) *");

       if (boost::regex_match(str, matches, pattern)) {
           *filter = new LevFilter(matches[1], matches[2], std::stoul(matches[3]));
           return true;
       }
       return false;
   }

   bool Test(const VariableSet&& variableSet, const VariableSetRow&& values) override {
	   std::size_t aa = variableSet.indexOf(_variable);
       std::string str = std::dynamic_pointer_cast<model::types::String, model::types::Base>(values.at(aa).dataPointer())->value();
       return levenshtein_distance(str, _target) <= _maxdist;
   }

};


#endif
