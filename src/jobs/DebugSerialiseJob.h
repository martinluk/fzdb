#ifndef JOBS_DEBUGSERIALISEJOB_H
#define JOBS_DEBUGSERIALISEJOB_H

#include "../session.h"
#include "../Job.h"
#include "QueryResult.h"

class DebugSerialiseJob : public Job
{
public:
	DebugSerialiseJob(ISession* session);

	// Inherited via Job
	virtual QueryResult execute() override;
};

#endif	// JOBS_DEBUGSERIALISEJOB_H
