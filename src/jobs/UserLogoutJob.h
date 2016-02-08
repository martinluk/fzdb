#pragma once
#include "../session.h"
#include "../Job.h"

#include "QueryResult.h"

class UserLogoutJob : public Job {
public:
	UserLogoutJob(std::shared_ptr<ISession> session);
	virtual QueryResult execute() override;

private:
	std::shared_ptr<ISession> _session;
};

