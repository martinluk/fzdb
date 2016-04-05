#ifndef FUZZY_FILTER_ORDERINGFILTERS
#define FUZZY_FILTER_ORDERINGFILTERS

#include "./IFilter.h"
#include "../types/Int.h"

#include <string>
#include <boost/regex.hpp>
#include <iostream>

class GreaterThanFilter : public IFilter {
private:
   const int _value;   
   const std::string _variable;
public:

   static bool TestAndCreate(IFilter** filter, std::string str) {
      boost::smatch matches;
      const static boost::regex pattern(" *\\$([a-zA-z0-9]+) *> *([0-9]+) *");

      if(boost::regex_match(str, matches, pattern)) {
         *filter = new GreaterThanFilter(matches[1], std::stoi(matches[2])); 
         std::cout << "Got here!" << std::endl;        
         return true;      
      }
      return false;
   }

   GreaterThanFilter(const std::string variable, const int value) : _variable(variable), _value(value) {}

   bool Test(const VariableSet&& variableSet, const VariableSetRow&& values) override {
       unsigned char aa = variableSet.indexOf(_variable);
       int val = std::dynamic_pointer_cast<model::types::Int, model::types::Base>(values.at(aa).dataPointer())->value();
        return val > _value;
   }

};


#endif
