#include "./IUserAdminJobs.h"

class PromoteEditorJob : public IUserAdminJobs {
	public:
		PromoteEditorJob(std::shared_ptr<ISession> session, std::string username);
        QueryResult adminJobBody() override;
	private:
		std::string _username;
};
