#include "DemoteAdminJob.h"
#include "../user/UserAttributes.h"
#include "../user/Hashing.h"
#include "../user/UserOperation.h"

DemoteAdminJob::DemoteAdminJob(std::shared_ptr<ISession> session, std::string username):IUserAdminJobs(session) {
	_username = username;
}

QueryResult DemoteAdminJob::adminJobBody() {
    QueryResult result;
    try {
        UserGroup group = UserOperation::getUserGroup(_username); //Throws user not exist exception
        if (group != UserGroup::ADMIN) {
        	throw std::runtime_error("Error: given user is not an admin, cannot promote to admin");
		}
    } catch (std::exception exception) {
        result.generateError(exception.what());
        return result;
    }
	UserOperation::changeUserGroup(_username, UserGroup::EDITOR);
    result.setValue("status","0");
    return result;
}
