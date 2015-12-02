#ifndef JOBS_DEBUGSERIALISEJOB_H
#define JOBS_DEBUGSERIALISEJOB_H

#include "../session.h"
#include "../Job.h"

class DebugSerialiseJob : public Job
{
public:
	DebugSerialiseJob(TCPSession* session);

	// Inherited via Job
	virtual void execute() override;
};

#endif	// JOBS_DEBUGSERIALISEJOB_H
