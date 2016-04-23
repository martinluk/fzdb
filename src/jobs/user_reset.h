#ifndef FUZZY_JOBS_RESET_PASSWORD
#define FUZZY_JOBS_RESET_PASSWORD

#include "../job.h"

class ResetPasswordJob : public Job {

    /**
     * @brief Resets password of other users - runnable by admin
     */
    public:

        /**
         * @brief Resets password of other users - runnable by admin
         *
         * @param session Session object of the current session.
         * @param username Username of the user that wished to be reset.
         * @param password The new password.
         */
        ResetPasswordJob(std::shared_ptr<ISession> session,
                const std::string &username,
                const std::string &password);
        
        virtual bool constOperation() const override { return false; }
        virtual QueryResult executeNonConst() override;
        
    private:
        std::string _username;
        std::string _password;
};

#endif
