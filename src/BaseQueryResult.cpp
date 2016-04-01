#include "BaseQueryResult.h"

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

BaseQueryResult::BaseQueryResult()
{
    _document.SetObject();
}

std::string BaseQueryResult::toJson() const
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    _document.Accept(writer);    
    return buffer.GetString();
}

void BaseQueryResult::setValue(const std::string &key, const std::string &value)
{
    rapidjson::Value k(key.c_str(), allocator());
    rapidjson::Value v(value.c_str(), allocator());
    setValue(k,v);
}

void BaseQueryResult::setValue(const std::string &key, rapidjson::Value &value)
{
    rapidjson::Value k(key.c_str(), allocator());
    setValue(k,value);
}

void BaseQueryResult::setValue(const std::string &key, bool value)
{
    rapidjson::Value k(key.c_str(), allocator());
    rapidjson::Value v(value);
    setValue(k,v);
}

void BaseQueryResult::setValue(const std::string &key, int value)
{
    rapidjson::Value k(key.c_str(), allocator());
    rapidjson::Value v(value);
    setValue(k,v);
}

void BaseQueryResult::setValue(rapidjson::Value &key, rapidjson::Value &value)
{
    _document.AddMember(key, value, allocator());
}


rapidjson::MemoryPoolAllocator<>& BaseQueryResult::allocator()
{
    return _document.GetAllocator();
}

std::string BaseQueryResult::getValue(const std::string &key) const
{
    return _document[key.c_str()].GetString();
}
