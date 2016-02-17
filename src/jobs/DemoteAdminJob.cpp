#include "DemoteAdminJob.h"
#include "../user/UserAttributes.h"
#include "../user/Hashing.h"
#include "../user/UserOperation.h"

DemoteAdminJob::DemoteAdminJob(std::shared_ptr<ISession> session, const std::string &username):IUserAdminJobs(session) {
	_username = username;
}

QueryResult DemoteAdminJob::executeNonConst()
{
	if ( !hasAdminPermissions() )
		return errorNoAdminPermissions();
	
    try {
        Permission::UserGroup group = _database->users().getUserGroup(_username); //Throws user not exist exception
        if (group != Permission::UserGroup::ADMIN) {
        	throw std::runtime_error("Error: given user is not an admin, cannot promote to admin");
		}
    } catch (std::exception exception) {
        return QueryResult::generateError(exception.what());
    }
	
	_database->users().changeUserGroup(_username, Permission::UserGroup::EDITOR);
	
	QueryResult result;
    result.setValue("status","0");
    return result;
}
