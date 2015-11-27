#ifndef JOBS_CREATEENTITYJOB_H
#define JOBS_CREATEENTITYJOB_H

#include "../Job.h"

class CreateEntityJob : public Job
{
public:
	CreateEntityJob(TCPSession* session);

	virtual void execute() override;
};

#endif	// JOBS_CREATEENTITYJOB_H
