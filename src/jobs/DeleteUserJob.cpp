#include "DeleteUserJob.h"
#include "../user/UserAttributes.h"
#include "../user/Hashing.h"
#include "../user/UserOperation.h"

DeleteUserJob::DeleteUserJob(ISession* session, std::string username):IUserAdminJobs(session) {
	_username = username;
}

QueryResult DeleteUserJob::execute() { 
	UserOperation::removeUser(UserGroup::ADMIN, _username);
	QueryResult result; 
	//TODO respond with some kind of success
	return result;
}
