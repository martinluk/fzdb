#include "./load_file.h"
#include "./save_file.h"

#include <boost/algorithm/string.hpp>

#include "../file_system.h"
#include "../singletons.h"
#include "../model/entity_manager.h"

LoadFileJob::LoadFileJob(std::shared_ptr<ISession> session, const std::string &message) : Job(session, PermType::UserOp)
{
    _message = message;
}

SaveFileJob::SaveFileJob(std::shared_ptr<ISession> session, const std::string &message) : Job(session, PermType::UserOp)
{
    _message = message;
}

QueryResult _result(const std::string &message)
{
    QueryResult result;
    result.setResultDataText(message);
    return result;
}

bool _preprocess(std::string &message)
{
    boost::algorithm::trim(message);
    
    if ( message.size() < 1 )
    {
        return false;
    }
    
    if (message[0] == '\"')
    {
        message.erase(0,1);
    }
    
    if ( message.size() < 1 )
    {
        return false;
    }
    
    if ( message[message.size()-1] == '\"' )
    {
        message.erase(message.size()-1, 1);
    }
    
    if ( message.size() < 1 )
    {
        return false;
    }
    
    // If the first character is not '/', assume the file path is local.
    if ( message[0] != '/' )
    {
        message = FileSystem::workingDirectory()  + std::string("/") + message;
    }
    
    return true;
}

QueryResult LoadFileJob::executeNonConst()
{
    if ( !_preprocess(_message) )
    {
        return QueryResult::generateError(QueryResult::ErrorCode::FileSystemError, std::string("Invalid file name."));
    }
    
    bool success = _database->entityManager().loadFromFile(_message);
    if ( success )
    {
        return _result(std::string("Data successfully loaded from ") + _message);
    }
    else
    {
        return QueryResult::generateError(QueryResult::ErrorCode::FileSystemError, std::string("Unable to read data from ") + _message);
    }
}

QueryResult SaveFileJob::executeConst() const
{
    if ( !_preprocess(_message) )
    {
        return QueryResult::generateError(QueryResult::ErrorCode::FileSystemError, std::string("Invalid file name."));
    }
    
    bool success = _database->entityManager().saveToFile(_message);
    if ( success )
    {
        return _result(std::string("Data successfully saved to ") + _message);
    }
    else
    {
        return QueryResult::generateError(QueryResult::ErrorCode::FileSystemError, std::string("Unable to save data to ") + _message);
    }
}
