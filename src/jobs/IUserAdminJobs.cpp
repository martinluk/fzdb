#include "IUserAdminJobs.h"

IUserAdminJobs::IUserAdminJobs(ISession* session):Job(session) {
    _currentUserGroup = session->getCurrentUserUserGroup();
}

QueryResult IUserAdminJobs::execute() {
        QueryResult qryResult;
        //Get Current User Group
        //Make sure the login is correct
        if (_currentUserGroup!=UserGroup::ADMIN) {
            return qryResult.generateError("Running an admin job without admin permission.");
        }
        adminJobBody();
}
