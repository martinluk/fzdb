#include "PromoteEditorJob.h"
#include "../user/UserAttributes.h"
#include "../user/Hashing.h"
#include "../user/UserOperation.h"
#include <stdexcept>

PromoteEditorJob::PromoteEditorJob(std::shared_ptr<ISession> session, const std::string &username):IUserAdminJobs(session) {
	_username = username;
}

QueryResult PromoteEditorJob::executeNonConst()
{
	if ( !hasAdminPermissions() )
		return errorNoAdminPermissions();
	
    try {
        Permission::UserGroup group = _database->users().getUserGroup(_username); //Throws user not exist exception
        if (group != Permission::UserGroup::EDITOR) {
        	return QueryResult::generateError(QueryResult::ErrorCode::UserDataError, "User to promote is not an editor.");
		}
    } catch (const std::exception &ex) {
        return QueryResult::generateError(QueryResult::ErrorCode::UserDataError, ex.what());
    }
	
	_database->users().changeUserGroup(_username, Permission::UserGroup::ADMIN);
	
	QueryResult result;
    result.setResultDataText(std::string("User ") + _username + std::string(" promoted to admin."));
    return result;
}
