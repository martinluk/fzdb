#ifndef JOBS_CREATEENTITYJOB_H
#define JOBS_CREATEENTITYJOB_H

#include "../Job.h"
#include "QueryResult.h"

class CreateEntityJob : public Job
{
public:
	CreateEntityJob(ISession* session);

	virtual QueryResult execute() override;
};

#endif	// JOBS_CREATEENTITYJOB_H
