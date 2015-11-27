#ifndef FUZZYDB_JOB
#define FUZZYDB_JOB

#include "ISession.h"

class Job 
{
public:
	Job(ISession* session) 
	{
		_session = session;
	}

	virtual ~Job() {}

	virtual void execute() = 0;
protected:
	ISession* _session;
};

#endif
