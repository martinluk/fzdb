#pragma once
#include "../session.h"
#include "../job.h"

#include "../query_result.h"

// Logs a user into the database.
class UserLoginJob : public Job {
public:
    typedef Permission::PermissionType PermType;
    UserLoginJob(std::shared_ptr<ISession> session, const std::string &username, const std::string &password);
    virtual bool constOperation() const override { return false; }
    virtual QueryResult executeNonConst() override;

private:
    std::string _username;
    std::string _password;
    std::shared_ptr<ISession> _session;
};

