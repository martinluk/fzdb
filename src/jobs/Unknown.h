#ifndef JOBS_UNKNOWNJOB_H
#define JOBS_UNKNOWNJOB_H

#include "../session.h"

#include "../Job.h"

#include "QueryResult.h"

class UnknownJob : public Job
{
private:
  std::string _name;

public:
	UnknownJob(std::shared_ptr<ISession> session, std::string name);
	
	virtual bool constOperation() const override { return true; }
	virtual QueryResult executeConst() const;
};

#endif	// JOBS_UNKNOWNJOB_H
