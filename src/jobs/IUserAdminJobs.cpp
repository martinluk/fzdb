#include "IUserAdminJobs.h"

IUserAdminJobs::IUserAdminJobs(std::shared_ptr<ISession> session):Job(session) {
}

QueryResult IUserAdminJobs::execute() {
        QueryResult qryResult;
        //Get Current User Group
        //Make sure the login is correct
        if (Session()->getCurrentUserUserGroup() != Permission::UserGroup::ADMIN) {
            return qryResult.generateError("Running an admin job without admin permission.");
        }
        qryResult = IUserAdminJobs::adminJobBody();
        return qryResult;
}
