#ifndef BASEQUERYRESULT_H
#define BASEQUERYRESULT_H

#include <string>
#include <rapidjson/document.h>

// Base query result class.
// The QueryResult class relies on this class for doing the dirty
// work and actually serialising JSON data.
class BaseQueryResult
{
public:
    BaseQueryResult();
    
    std::string toJson() const;
    rapidjson::MemoryPoolAllocator<>& allocator();
    
protected:
    // Functions for setting values of different types.
    void setValue(const std::string &key, rapidjson::Value &value);
    void setValue(const std::string &key, const std::string &value);
    void setValue(const std::string &key, bool value);
    void setValue(const std::string &key, int value);
    void setValue(rapidjson::Value& key, rapidjson::Value& value);
    std::string getValue(const std::string &key) const;
    
    rapidjson::Document _document;
};

#endif // BASEQUERYRESULT_H
