#pragma once
#include "../Job.h"
#include "../user/Permission.h"
#include "../QueryResult.h"

// Shared base class for user management jobs.
class IUserAdminJobs : public Job {
    public:
       IUserAdminJobs(std::shared_ptr<ISession> session);
       
    protected:
        bool hasAdminPermissions() const;
        static QueryResult errorNoAdminPermissions();
};
