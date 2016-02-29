#include "Job.h"
#include "singletons.h"

#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

Job::Job(std::shared_ptr<ISession> session)
{
	_session = session;
	_database = NULL;
}

QueryResult Job::execute()
{
	QueryResult result;
	
	if ( constOperation() )
	{
		// Get a shared lock - no limit on these.
		boost::shared_lock<boost::shared_mutex> lock(Singletons::databaseMutex());
		
		_database = Singletons::database();
		result = executeConst();
		_database = NULL;
	}
	else
	{
		// Get an exclusive lock for writing - there can only be one of these.
		// Only a single thread can acquire an upgrade_lock at one time.
		boost::upgrade_lock<boost::shared_mutex> lock(Singletons::databaseMutex());
		boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
		
		_database = Singletons::database();
		result = executeNonConst();
		_database = NULL;
	}
	
	return result;
}
