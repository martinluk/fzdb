#ifndef JOBS_SAVEFILEJOB_H
#define JOBS_SAVEFILEJOB_H

#include "../Job.h"
#include "QueryResult.h"

class SaveFileJob : public Job
{
public:
	SaveFileJob(std::shared_ptr<ISession> session, const std::string &message);

	virtual QueryResult execute() override;
	
private:
	std::string	_message;
};

#endif	// JOBS_SAVEFILEJOB_H
