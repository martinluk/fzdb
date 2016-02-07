#include "PromoteEditorJob.h"
#include "../user/UserAttributes.h"
#include "../user/Hashing.h"
#include "../user/UserOperation.h"

PromoteEditorJob::PromoteEditorJob(ISession* session, std::string username):IUserAdminJobs(session) {
	_username = username;
}

QueryResult PromoteEditorJob::adminJobBody() {
	UserOperation::changeUserGroup(_username, UserGroup::ADMIN);
	QueryResult result; 
	//TODO respond with some kind of success
	return result;
}
