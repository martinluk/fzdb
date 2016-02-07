#include "IUserAdminJobs.h"

IUserAdminJobs::IUserAdminJobs(ISession* session):Job(session) {
    _session=session;
}

QueryResult IUserAdminJobs::execute() {
        QueryResult qryResult;
        //Get Current User Group
        UserGroup usergroup = _session.getCurrentUserUserGroup();
        //Make sure the login is correct
        if (usergroup!=UserGroup::ADMIN) {
            return qryResult.generateError("Running an admin job without admin permission.");
        }
        adminJobBody();
}
