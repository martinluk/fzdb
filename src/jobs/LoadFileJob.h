#ifndef JOBS_LOADFILEJOB_H
#define JOBS_LOADFILEJOB_H

#include "../Job.h"
#include "QueryResult.h"

// Loads a specified database file from disk.
class LoadFileJob : public Job
{
public:
    LoadFileJob(std::shared_ptr<ISession> session, const std::string &message);

    virtual bool constOperation() const override { return false; }
    virtual QueryResult executeNonConst() override;
    
private:
    std::string    _message;
};

#endif    // JOBS_LOADFILEJOB_H
