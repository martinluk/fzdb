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

  boost::property_tree::ptree getElement(std::string key) {
	  return _ptree.get_child(key);
  }

  bool hasValue(std::string key) {
	  return _ptree.find(key) != _ptree.not_found();
  }

  boost::property_tree::ptree root() {
	  return _ptree;
  }

  std::string toJSON();
};

#endif