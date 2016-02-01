#include "AddUserJob.h"
#include "../user/UserAttributes.h"
#include "../user/Hashing.h"
#include "../user/UserOperation.h"

AddUserJob::AddUserJob(ISession* session, std::string username, std::string password):IUserAdminJobs(session) {
	_username = username;
	_password = password;
}

QueryResult AddUserJob::execute() { 
	UserOperation::addUser(UserGroup::ADMIN, _username, _password, UserGroup::EDITOR);
	QueryResult result; 
	//TODO respond with some kind of success
	return result;
}
