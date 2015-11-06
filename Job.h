#ifndef FUZZYDB_JOB
#define FUZZYDB_JOB

#include "session.h"

class Job 
{
public:
	Job(TCPSession* session) 
	{
		_session = session;
	}

	virtual void execute() = 0;
protected:
	TCPSession* _session;
};

#endif