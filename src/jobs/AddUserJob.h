#include "./IUserAdminJobs.h"

class AddUserJob : public IUserAdminJobs {
	public:
		AddUserJob(std::shared_ptr<ISession> session, std::string username, std::string password);
        QueryResult adminJobBody() override;
	private:
		std::string _username;
		std::string _password;
};
