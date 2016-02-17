#include "QueryResult.h"

const std::string QueryResult::PROPERTY_STATUS("status");
const std::string QueryResult::PROPERTY_ERRORCODE("errorCode");
const std::string QueryResult::PROPERTY_INFO("info");
const std::string QueryResult::PROPERTY_RESULT("result");
const std::string QueryResult::PROPERTY_RESULTTYPE("type");
const std::string QueryResult::PROPERTY_RESULTDATA("data");

QueryResult::QueryResult() : BaseQueryResult()
{
	setValue(PROPERTY_STATUS, true);
	setValue(PROPERTY_ERRORCODE, (int)ErrorCode::NoError);
	setValue(PROPERTY_INFO, std::string());
	
	rapidjson::Value obj(rapidjson::kObjectType);
	setValue(PROPERTY_RESULT, obj);
}

bool QueryResult::status() const
{
	return _document[PROPERTY_STATUS.c_str()].GetBool();
}

void QueryResult::setStatus(bool success)
{
	_document[PROPERTY_STATUS.c_str()].SetBool(success);
}

QueryResult::ErrorCode QueryResult::errorCode() const
{
	// Not particularly safe
	return (ErrorCode)_document[PROPERTY_ERRORCODE.c_str()].GetInt();
}

void QueryResult::setErrorCode(ErrorCode code)
{
	if ( code == ErrorCode::NoError )
		setStatus(true);
	else
		setStatus(false);
	_document[PROPERTY_ERRORCODE.c_str()].SetInt((int)code);
}

std::string QueryResult::info() const
{
	return std::string(_document[PROPERTY_INFO.c_str()].GetString());
}

void QueryResult::setInfo(const std::string &str)
{
	_document[PROPERTY_INFO.c_str()].SetString(str.c_str(), allocator());
}

std::string QueryResult::resultType() const
{
	const rapidjson::Value& resultObj = _document[PROPERTY_RESULT.c_str()];
	if ( resultObj.ObjectEmpty() )
		return std::string();
	
	return std::string(resultObj[PROPERTY_RESULTTYPE.c_str()].GetString());
}

void QueryResult::setResultType(const std::string &type)
{
	rapidjson::Value& resultObj = _document[PROPERTY_RESULT.c_str()];
	if ( !resultObj.HasMember(PROPERTY_RESULTTYPE.c_str()) )
	{
		rapidjson::Value k(PROPERTY_RESULTTYPE.c_str(), allocator());
		
		rapidjson::Value str;
		str = "";
		resultObj.AddMember(k, str, allocator());
	}
	
	resultObj[PROPERTY_RESULTTYPE.c_str()].SetString(type.c_str(), allocator());
}

std::string QueryResult::resultDataText() const
{
	const rapidjson::Value& resultObj = _document[PROPERTY_RESULT.c_str()];
	if ( resultObj.ObjectEmpty() )
		return std::string();
	
	const rapidjson::Value &dataObj = resultObj[PROPERTY_RESULTDATA.c_str()];
	if ( !dataObj.IsString() )
		return std::string();
	
	return std::string(dataObj.GetString());
}

void QueryResult::setResultDataText(const std::string &data)
{
	rapidjson::Value& resultObj = _document[PROPERTY_RESULT.c_str()];
	if ( !resultObj.HasMember(PROPERTY_RESULTDATA.c_str()) )
	{
		rapidjson::Value k(PROPERTY_RESULTDATA.c_str(), allocator());
		
		rapidjson::Value str;
		str = "";
		resultObj.AddMember(k, str, allocator());
	}
	
	resultObj[PROPERTY_RESULTDATA.c_str()].SetString(data.c_str(), allocator());
}

void QueryResult::setResultDataFsparql(rapidjson::Value &data)
{
	rapidjson::Value& resultObj = _document[PROPERTY_RESULT.c_str()];
	if ( resultObj.HasMember(PROPERTY_RESULTDATA.c_str()) )
	{
		resultObj.RemoveMember(PROPERTY_RESULTDATA.c_str());
	}
	
	rapidjson::Value k(PROPERTY_RESULTDATA.c_str(), allocator());
	resultObj.AddMember(k, data, allocator());
}

QueryResult QueryResult::generateError(ErrorCode code, const std::string &errorString)
{
	QueryResult result;
	result.setInfo(errorString);
	result.setErrorCode(code);
	return result;
}
