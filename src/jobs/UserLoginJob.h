#pragma once
#include "../session.h"
#include "../Job.h"

#include "QueryResult.h"

class UserLoginJob : public Job {
public:
	UserLoginJob(std::shared_ptr<ISession> session, std::string username, std::string password);
	virtual QueryResult execute() override;

private:
	std::string _username;
	std::string _password;
	std::shared_ptr<ISession> _session;
};

