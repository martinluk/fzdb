#ifndef FUZZY_QUERYRESULTS
#define FUZZY_QUERYRESULTS

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

class QueryResult
{
private:
  boost::property_tree::ptree _ptree;
public:
  QueryResult();

  template<typename T>
  void setValue(std::string key, T value) {
	  _ptree.put<T>(key, value);
  }

  template<typename T>
  T getValue(std::string key) {
	  return _ptree.get_child(key).get_value<T>();
  }

  std::string toJSON();
};

#endif