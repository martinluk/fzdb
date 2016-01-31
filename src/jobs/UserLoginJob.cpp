#include "UserLoginJob.h"

#include "../user/UserOperation.h"

UserLoginJob::UserLoginJob(ISession* session, std::string username, std::string password) : Job(session) {
	_username=username;
	_password=password;
};

QueryResult UserLoginJob::execute() {
	QueryResult result;
	try {
		UserOperation::login(_username,_password);
	} catch (LoginUnsuccessfulException ex) { 
		result = QueryResult::generateError(ex.what());
	}
	return result;
}
