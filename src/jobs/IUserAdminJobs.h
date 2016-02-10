#pragma once
#include "../Job.h"
#include "../ISession.h"
#include "../user/Permission.h"

class IUserAdminJobs : public Job {
	public:
       IUserAdminJobs(std::shared_ptr<ISession> session);
       QueryResult execute() override ;
    protected:
        virtual QueryResult adminJobBody() {}
};
