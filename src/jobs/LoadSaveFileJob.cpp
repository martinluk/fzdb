#include "LoadFileJob.h"
#include "SaveFileJob.h"

#include <boost/algorithm/string.hpp>

#include "../FileSystem.h"
#include "../singletons.h"
#include "../model/EntityManager.h"

LoadFileJob::LoadFileJob(ISession *session, const std::string &message) : Job(session)
{
	_message = message;
}

SaveFileJob::SaveFileJob(ISession *session, const std::string &message) : Job(session)
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

QueryResult LoadFileJob::execute()
{
	if ( !_preprocess(_message) )
	{
		return _result(std::string("Invalid file name."));
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

QueryResult SaveFileJob::execute()
{
	if ( !_preprocess(_message) )
	{
		return _result(std::string("Invalid file name."));
	}
	
	bool success = Singletons::entityManager()->saveToFile(_message);
	if ( success )
	{
		return _result(std::string("Data successfully saved to ") + _message);
	}
	else
	{
		return _result(std::string("Unable to save data to ") + _message);
	}
}
