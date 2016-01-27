#include "LoadFileJob.h"

#include <boost/algorithm/string.hpp>

#include "../FileSystem.h"
#include "../singletons.h"
#include "../model/EntityManager.h"

LoadFileJob::LoadFileJob(ISession *session, const std::string &message) : Job(session)
{
	_message = message;
}

QueryResult _result(const std::string &message)
{
	QueryResult result;
    result.setValue("type", "string");
    result.setValue(std::string("response"), message);
    return result;
}

QueryResult LoadFileJob::execute()
{
	boost::algorithm::trim_left(_message);
	boost::algorithm::trim_right(_message);
	
	if ( _message.size() < 1 )
	{
		return _result(std::string("Invalid file name."));
	}
	
	if (_message[0] == '\"')
	{
		_message.erase(0,1);
	}
	
	if ( _message.size() < 1 )
	{
		return _result(std::string("Invalid file name."));
	}
	
	if ( _message[_message.size()-1] == '\"' )
	{
		_message.erase(_message.size()-1, 1);
	}
	
	if ( _message.size() < 1 )
	{
		return _result(std::string("Invalid file name."));
	}
	
	// If the first character is not '/', assume the file path is local.
	if ( _message[0] != '/' )
	{
		_message = FileSystem::workingDirectory()  + std::string("/") + _message;
	}
	
	bool success = Singletons::entityManager()->loadFromFile(_message);
	if ( success )
	{
		return _result(std::string("Data successfully loaded from ") + _message);
	}
	else
	{
		return _result(std::string("Unable to read data from ") + _message);
	}
}
