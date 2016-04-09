#include "Job.h"
#include "singletons.h"

#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

Job::Job(std::shared_ptr<ISession> session, PermType permtype)
{
    _session = session;
    _database = NULL;
    _permtype = permtype;
}

QueryResult Job::execute()
{
    QueryResult result;

    // Jonathan: This is a bit of a hack, really. Listen for any uncaught exceptions and return a generic error.
    // Really we should define a base exception class that allows the query result error code to be
    // propagated back, and then not do any exception handling at all in any of the job subclasses
    // to allow all exceptions to be caught here.
    // Still, better to return a generic error code than for the database to crash out.
    try
    {
        //Check session permission matches with the the session
        std::string username = _session->username();
        //Read user group
        Permission::UserGroup currentUsergroup= _session->getUserGroup();
        bool hasPermission= Permission::checkPermission(currentUsergroup, _permtype);

        if(!hasPermission) {
            result.setErrorCode(QueryResult::ErrorCode::InsufficientPermissions);
            result.setInfo("Insufficient permission to run the job");
            //TODO Make more informative information, 
            //Perhaps including job name(?), current perm and least perm (?)
            return result;
        }

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
    }
    catch ( const std::exception &e )
    {
        result.setErrorCode(QueryResult::ErrorCode::UnhandledError);
        result.setInfo(e.what());
    }
    
    return result;
}
