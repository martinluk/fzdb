#pragma once
#include "../isession.h"
#include "../session.h"
#include "../job.h"

#include "../query_result.h"

// Logs a currently logged-in user out of the database.
class UserLogoutJob : public Job {
public:
    UserLogoutJob(std::shared_ptr<ISession> session);
    
    virtual bool constOperation() const override { return true; }
    virtual QueryResult executeConst() const override;

private:
    std::shared_ptr<ISession> _session;
};

