#include "../session.h"
#include "../user/UserLogin.h"
#include "../Job.h"

#include "QueryResult.h"

class UserLoginJob : public Job {
public:
	UserLoginJob(ISession* session, std::string username, std::string password);
	virtual QueryResult execute() override;

private:
	std::string _username;
	std::string _password;
};

