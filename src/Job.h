#ifndef FUZZYDB_JOB
#define FUZZYDB_JOB

#include "ISession.h"
#include "QueryResult.h"

class Job 
{
public:
	Job(ISession* session) 
	{
		_session = session;
	}

	virtual ~Job() {}

	virtual QueryResult execute() = 0;

  ISession* Session() {
    return _session;
  }
protected:
	ISession* _session;
};

#endif
