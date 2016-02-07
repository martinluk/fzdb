#include "UserLogoutJob.h"

//User folder includes
#include "../user/Permission.h"
#include "../user/UserOperation.h"

UserLogoutJob::UserLogoutJob(ISession* session) : Job(session) {
	_session=session;
}

QueryResult UserLogoutJob::execute() {
	QueryResult result;
	//TODO Some kind of exception of user is not already login?
	_session->clearCurrentUserName();
	return result;
}
