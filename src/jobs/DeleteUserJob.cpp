#include "DeleteUserJob.h"
#include "../user/UserAttributes.h"
#include "../user/Hashing.h"
#include "../user/UserOperation.h"
#include "../user/UserExceptions.h"

DeleteUserJob::DeleteUserJob(std::shared_ptr<ISession> session, const std::string &username):IUserAdminJobs(session) {
	_username = username;
}

QueryResult DeleteUserJob::executeNonConst()
{
	if ( !hasAdminPermissions() )
		return errorNoAdminPermissions();
	
    try {
		_database->users().removeUser(_username);
    } catch (const std::exception &ex) {
        return QueryResult::generateError(QueryResult::ErrorCode::UserDataError, ex.what());
    }
	
	QueryResult result;
    result.setResultDataText(std::string("User ") + _username + std::string("Deleted successfully."));
    return result;
}
