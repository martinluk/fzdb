#ifndef FUZZY_QUERYRESULTS
#define FUZZY_QUERYRESULTS

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

class QueryResults 
{
private:
  boost::property_tree::ptree _ptree;
public:
  QueryResults();

  template<typename T>
  void setValue(std::string key, T value);

  template<typename T>
  T getValue(std::string key);

  std::string toJSON();
};

#endif