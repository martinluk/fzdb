#ifndef JOBS_SAVEFILEJOB_H
#define JOBS_SAVEFILEJOB_H

#include "../job.h"
#include "../query_result.h"


/**
* @brief Saves the current database state to a file. 
*/
class SaveFileJob : public Job {
public:
    SaveFileJob(std::shared_ptr<ISession> session, const std::string &message);

    virtual bool constOperation() const override { return true; }
    virtual QueryResult executeConst() const override;
    
private:
    mutable std::string    _message;
};

#endif    // JOBS_SAVEFILEJOB_H
