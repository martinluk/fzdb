#ifndef FUZZYDB_JOB
#define FUZZYDB_JOB

#include "ISession.h"
#include "QueryResult.h"

class Job 
{
public:
	Job(std::shared_ptr<ISession> session) 
	{
		_session = session;
	}

	virtual ~Job() {}

	virtual QueryResult execute() = 0;

	std::shared_ptr<ISession> Session() {
		return _session;
	}
protected:
	std::shared_ptr<ISession> _session;
};

#endif
