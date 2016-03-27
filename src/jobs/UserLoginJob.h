#pragma once
#include "../session.h"
#include "../Job.h"

#include "QueryResult.h"

class UserLoginJob : public Job {
public:
    UserLoginJob(std::shared_ptr<ISession> session, const std::string &username, const std::string &password);
    
    virtual bool constOperation() const override { return false; }
    virtual QueryResult executeNonConst() override;

private:
    std::string _username;
    std::string _password;
    std::shared_ptr<ISession> _session;
};

