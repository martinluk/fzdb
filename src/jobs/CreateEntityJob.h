#ifndef JOBS_CREATEENTITYJOB_H
#define JOBS_CREATEENTITYJOB_H

#include "../Job.h"
#include "QueryResult.h"
#include <string>

class CreateEntityJob : public Job
{
public:
	CreateEntityJob(std::shared_ptr<ISession> session, const std::string &type);

	virtual QueryResult execute() override;
	
private:
	std::string _type;
};

#endif	// JOBS_CREATEENTITYJOB_H
