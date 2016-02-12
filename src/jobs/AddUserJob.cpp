#include "AddUserJob.h"
#include "../user/UserAttributes.h"
#include "../user/Hashing.h"
#include "../user/UserOperation.h"
#include "../user/UserExceptions.h"

AddUserJob::AddUserJob(std::shared_ptr<ISession> session, std::string username, std::string password):IUserAdminJobs(session) {
	_username = username;
	_password = password;
}

QueryResult AddUserJob::adminJobBody() {
	QueryResult result; 
    try {
        //TODO Need to verify username and password are not empty.
        Singletons::database()->users().addUser(_username, _password, Permission::UserGroup::EDITOR);
    } catch (const std::exception &ex) {
        result.generateError(ex.what());
        return result;
    }
    result.setValue("status","0");
	return result;
}
