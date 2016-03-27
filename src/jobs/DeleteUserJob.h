#include "./IUserAdminJobs.h"

// Deletes a user from the database. Requires admin privileges.
class DeleteUserJob : public IUserAdminJobs {
    public:
        DeleteUserJob(std::shared_ptr<ISession> session, const std::string &username);
        
        virtual bool constOperation() const override { return false; }
        virtual QueryResult executeNonConst() override;
        
    private:
        std::string _username;
};
