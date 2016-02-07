#include "AddUserJob.h"
#include "../user/UserAttributes.h"
#include "../user/Hashing.h"
#include "../user/UserOperation.h"
#include "../user/UserExceptions.h"

AddUserJob::AddUserJob(ISession* session, std::string username, std::string password):IUserAdminJobs(session) {
	_username = username;
	_password = password;
}

QueryResult AddUserJob::adminJobBody() {
	QueryResult result; 
    try {
        UserOperation::addUser(_username, _password, UserGroup::EDITOR);

    } catch (UserAlreadyExistException exception) {
        result.generateError("User already exist");
        return result;
    }
    result.setValue("status","0");
	return result;
}
