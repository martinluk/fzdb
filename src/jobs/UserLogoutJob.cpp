#include "UserLogoutJob.h"

//User folder includes
#include "../user/Permission.h"
#include "../user/UserOperation.h"

UserLogoutJob::UserLogoutJob(std::shared_ptr<ISession> session) : Job(session) {
	_session=session;
}

QueryResult UserLogoutJob::executeConst() const
{
	//TODO Some kind of exception of user is not already logged in?
	_session->clearCurrentUserName();
	
	QueryResult result;
	result.setResultDataText("Logged out successfully.");
    return result;
}
