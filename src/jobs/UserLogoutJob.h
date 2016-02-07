#pragma once
#include "../session.h"
#include "../Job.h"

#include "QueryResult.h"

class UserLogoutJob : public Job {
public:
	UserLogoutJob(ISession* session);
	virtual QueryResult execute() override;

private:
	ISession* _session;
};

