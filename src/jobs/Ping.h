#ifndef JOB_PINGJOB_H
#define JOB_PINGJOB_H
#include "../session.h"

#include "../Job.h"

#include "QueryResult.h"

class PingJob : public Job
{
public:

	PingJob(ISession* session);


	// Inherited via Job
	virtual QueryResult execute() override;

};

#endif	// JOB_PINGJOB_H
