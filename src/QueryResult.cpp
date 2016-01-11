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
