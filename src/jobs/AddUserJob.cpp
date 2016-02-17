#include "AddUserJob.h"
#include "../user/UserAttributes.h"
#include "../user/Hashing.h"
#include "../user/UserOperation.h"
#include "../user/UserExceptions.h"

AddUserJob::AddUserJob(std::shared_ptr<ISession> session, const std::string &username, const std::string &password):IUserAdminJobs(session) {
	_username = username;
	_password = password;
}

QueryResult AddUserJob::executeNonConst()
{
	if ( !hasAdminPermissions() )
		return errorNoAdminPermissions();
	
    try {
        //TODO Need to verify username and password are not empty.
        _database->users().addUser(_username, _password, Permission::UserGroup::EDITOR);
    } catch (const std::exception &ex) {
        return QueryResult::generateError(ex.what());
    }
	
	QueryResult result;
    result.setValue("status","0");
	return result;
}
