#ifndef JOBS_LOADFILEJOB_H
#define JOBS_LOADFILEJOB_H

#include "../Job.h"
#include "QueryResult.h"

class LoadFileJob : public Job
{
public:
	LoadFileJob(std::shared_ptr<ISession> session, const std::string &message);

	virtual QueryResult execute() override;
	
private:
	std::string	_message;
};

#endif	// JOBS_LOADFILEJOB_H
