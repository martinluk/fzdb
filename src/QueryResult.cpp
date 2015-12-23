#include "QueryResult.h"

QueryResult::QueryResult() {
  
}

std::string QueryResult::toJSON()
{
	std::stringstream ss;
	boost::property_tree::json_parser::write_json(ss, _ptree);
	return ss.str();
}
