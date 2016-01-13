#ifndef JOB_DEBUGJOB_H
#define JOB_DEBUGJOB_H
#include "../session.h"

#include "../Job.h"

#include "QueryResult.h"

// Generic debug job.
// Depending on the command arguments, we do various debug tasks here.
class DebugJob : public Job
{
public:
        DebugJob(ISession* session, std::string message);

        // Inherited via Job
        virtual QueryResult execute() override;

private:
        std::string _message;
};

#endif	// JOB_DEBUGJOB_H
