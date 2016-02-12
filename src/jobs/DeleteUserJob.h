#include "./IUserAdminJobs.h"

class DeleteUserJob : public IUserAdminJobs {
	public:
		DeleteUserJob(std::shared_ptr<ISession> session, std::string username);
        QueryResult adminJobBody() override;
	private:
		std::string _username;
};
