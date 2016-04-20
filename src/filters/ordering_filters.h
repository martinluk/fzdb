#ifndef FUZZY_FILTER_ORDERINGFILTERS
#define FUZZY_FILTER_ORDERINGFILTERS

#include "./ifilter.h"
#include "../types/ordered_type.h"

#include <string>
#include <boost/regex.hpp>
#include <iostream>

class OrderingFilter : public IFilter {
private:
	enum class Operator {
		EQUAL,
		LESSTHAN,
		GREATERTHAN,
		LESSTHANOREQUAL,
		GREATERTHANOREQUAL,
		NOTEQUAL
	};

   const std::string _value;   
   const std::string _variable;
   const Operator _operator;
public:
   static bool TestAndCreate(IFilter** filter, std::string str) {
      boost::smatch matches;
      const static boost::regex pattern(" *\\$([a-zA-z0-9]+) *(>|<|>=|<=|=|!=) *([0-9]+) *");

      if(boost::regex_match(str, matches, pattern)) {
		  Operator op;
		  if (matches[2] == "=") op = Operator::EQUAL;
		  else if (matches[2] == "<") op = Operator::LESSTHAN;
		  else if (matches[2] == ">") op = Operator::GREATERTHAN;
		  else if (matches[2] == "<=") op = Operator::LESSTHANOREQUAL;
		  else if (matches[2] == ">=") op = Operator::GREATERTHANOREQUAL;
		  else if (matches[2] == "!=") op = Operator::NOTEQUAL;
         *filter = new OrderingFilter(matches[1], op, matches[3]);
         std::cout << "Got here!" << std::endl;        
         return true;      
      }
      return false;
   }

   OrderingFilter(const std::string variable, const Operator op, const std::string value) : _variable(variable), _value(value), _operator(op) {}

   bool Test(const VariableSet&& variableSet, const VariableSetRow&& values) override {
	   std::size_t aa = variableSet.indexOf(_variable);
       std::shared_ptr<model::types::OrderedType> val = std::dynamic_pointer_cast<model::types::OrderedType, model::types::Base>(values.at(aa).dataPointer());
	   switch (_operator) {
	   case Operator::EQUAL:
		   return val->Equals(_value) != 0;
	   case Operator::NOTEQUAL:
		   return val->Equals(_value) == 0;
	   case Operator::LESSTHAN:
		   return val->lessThan(_value);
	   case Operator::GREATERTHAN:
		   return val->greaterThan(_value);
	   case Operator::LESSTHANOREQUAL:
		   return val->lessThan(_value) || val->Equals(_value) != 0;
	   case Operator::GREATERTHANOREQUAL:
		   return val->greaterThan(_value) || val->Equals(_value) != 0;
	   default:
		   // all enum cases have been considered
		   assert(false);
	   }
	   return false;
   }

};


#endif
