#ifndef FUZZY_QUERYRESULTS
#define FUZZY_QUERYRESULTS

#include <string>
#include <rapidjson\document.h>

class QueryResult
{
private:
  rapidjson::Document _document;
public:
  QueryResult();

  void setValue(const std::string key, std::string value) {
	  rapidjson::Value val, name;
	  val.SetString(value.c_str(), _document.GetAllocator());
	  name.SetString(key.c_str(), _document.GetAllocator());
	  _document.AddMember(name, val, _document.GetAllocator());
  }

 std::string getValue(std::string key) {
	 return _document[key.c_str()].GetString();
  }

  rapidjson::Document* root() {
	  return &_document;
  }

  std::string toJSON();
};

#endif