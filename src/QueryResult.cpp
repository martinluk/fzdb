#include "QueryResult.h"

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

QueryResult::QueryResult() {
	_document.SetObject();
}

std::string QueryResult::toJSON()
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	_document.Accept(writer);	
	return buffer.GetString();
}

QueryResult QueryResult::generateError(const std::string &errorString)
{
    QueryResult result;
    result.setValue("type", "string");
    result.setValue(std::string("response"), errorString);
    return result;
}
