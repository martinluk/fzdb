#include "DeleteUserJob.h"
#include "../user/UserAttributes.h"
#include "../user/Hashing.h"
#include "../user/UserOperation.h"
#include "../user/UserExceptions.h"

DeleteUserJob::DeleteUserJob(std::shared_ptr<ISession> session, std::string username):IUserAdminJobs(session) {
	_username = username;
}

QueryResult DeleteUserJob::adminJobBody() {
    QueryResult result;
    try {
		_database->users().removeUser(_username);
    } catch (const UserNotExistException &) {
        result.generateError("User does not exist");
        return result;
    }
    result.setValue("status","0");
    return result;
}
