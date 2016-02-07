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
        UserOperation::login(_username,_password);
	} catch (LoginUnsuccessfulException ex) { 
		result = QueryResult::generateError(ex.what());
        return result;
	}
    _session->setCurrentUserName(_username);
    result.setValue("status",0);
	return result;
}
