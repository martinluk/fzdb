#pragma once
#include "../Job.h"
#include "../ISession.h"
#include "../user/Permission.h"

class IUserAdminJobs : public Job {
	public:
       IUserAdminJobs(ISession* session);
		
       QueryResult execute() override ;

    protected:

        virtual QueryResult adminJobBody();

private:
        ISession _session;


};
