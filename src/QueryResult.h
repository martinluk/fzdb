#ifndef FUZZY_QUERYRESULTS
#define FUZZY_QUERYRESULTS

#include <string>
#include <rapidjson/document.h>

class QueryResult
{
private:
  rapidjson::Document _document;
public:
  QueryResult();

  void setValue(const std::string key, std::string value) {
	  rapidjson::Value val, name;
	  val.SetString(value.c_str(), allocator());
	  name.SetString(key.c_str(), allocator());
	  _document.AddMember(name, val, allocator());
  }

  void setValue(rapidjson::Value&& key, rapidjson::Value&& value) {
	  _document.AddMember(key, value, allocator());
  }

  rapidjson::MemoryPoolAllocator<>& allocator() {
	  return _document.GetAllocator();
  }

 std::string getValue(std::string key) {
	 return _document[key.c_str()].GetString();
  }

  rapidjson::Document& root() {
	  return _document;
  }

  std::string toJSON();

  static QueryResult generateError(const std::string &errorString);
};

#endif
