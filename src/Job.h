#ifndef FUZZYDB_JOB
#define FUZZYDB_JOB

#include "ISession.h"
#include "QueryResult.h"
#include <cassert>

class Database;

class Job 
{
public:
	Job(std::shared_ptr<ISession> session) ;
	virtual ~Job() {}

	QueryResult execute();
	
	// When writing new job:
	// - Implement constOperation() to signify whether or not the job modifies the database.
	// - If the operation is const, override executeConst().
	// - If the operation is not const, override executeNonConst().
	// - When accessing the database, ONLY use the _database protected member, not Singletons!
	// - If you need to modify one of your member variables in executeConst(), declare it as mutable.
	
	virtual QueryResult executeConst() const
	{
		assert(false);
	}
	virtual QueryResult executeNonConst()
	{
		assert(false);
	}
	virtual bool constOperation() const = 0;

	std::shared_ptr<ISession> Session() {
		return _session;
	}
protected:
	std::shared_ptr<ISession> _session;
	Database* _database;
};

#endif
