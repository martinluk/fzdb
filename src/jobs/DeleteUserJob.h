#include "./IUserAdminJobs.h"

class DeleteUserJob : public IUserAdminJobs {
    public:
        DeleteUserJob(std::shared_ptr<ISession> session, const std::string &username);
        
        virtual bool constOperation() const override { return false; }
        virtual QueryResult executeNonConst() override;
        
    private:
        std::string _username;
};
