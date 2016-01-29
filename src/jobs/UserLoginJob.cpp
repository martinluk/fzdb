#include "UserLoginJob.h"

UserLoginJob::UserLoginJob(ISession* session, std::string username, std::string password) : Job(session) {
	_username=username;
	_password=password;
};

QueryResult UserLoginJob::execute() {
	QueryResult result;
	try {
		UserCommonOperation::login(_username,_password);
	} catch (LoginUnsuccessfulException ex) { 
		result = QueryResult::generateError(ex.what());
	}
	return result;
}
