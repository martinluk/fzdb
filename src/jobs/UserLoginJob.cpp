#include "UserLoginJob.h"

//User folder includes
#include "../user/Permission.h"
#include "../user/UserOperation.h"

UserLoginJob::UserLoginJob(ISession* session, std::string username, std::string password) : Job(session) {
	_username=username;
	_password=password;
	_session=session;
};

QueryResult UserLoginJob::execute() {
	QueryResult result;
	try {
		UserGroup currentGroup = UserOperation::login(_username,_password);
		_session->setCurrentUserGroup(currentGroup);
	} catch (LoginUnsuccessfulException ex) { 
		result = QueryResult::generateError(ex.what());
	}
	return result;
}
