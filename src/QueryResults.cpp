#include "QueryResults.h"

QueryResults::QueryResults() {
  
}

std::string QueryResults::toJSON()
{
	std::stringstream ss;
	boost::property_tree::json_parser::write_json(ss, _ptree);
	return ss.str();
}

template<typename T>
void QueryResults::setValue(std::string key, T value) {
	_ptree.put<T>(key, value);
}

template<typename T>
T QueryResults::getValue(std::string key) {
	return _ptree.get_child(key).get_value<T>();
}
