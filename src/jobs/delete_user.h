#ifndef FUZZY_JOBS_DELETE_USER
#define FUZZY_JOBS_DELETE_USER

#include "../job.h"


class DeleteUserJob : public Job {

    /**
     * @brief Deletes a user from the database. Requires admin privileges.
     */
    public:
        DeleteUserJob(std::shared_ptr<ISession> session, const std::string &username);
        
        virtual bool constOperation() const override { return false; }
        virtual QueryResult executeNonConst() override;
        
    private:
        std::string _username;
};

#endif
