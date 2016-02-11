#pragma once
#include "../Job.h"
#include "../ISession.h"
#include "../user/Permission.h"
#include "../QueryResult.h"

class IUserAdminJobs : public Job {
	public:
       IUserAdminJobs(std::shared_ptr<ISession> session);
       QueryResult execute() override ;
    protected:
<<<<<<< HEAD
		virtual QueryResult adminJobBody() {
			return QueryResult();
		}
=======
		virtual QueryResult adminJobBody() { return QueryResult();  };
>>>>>>> origin/master
};
