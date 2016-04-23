#ifndef JOBS_DEBUGSAVEFILE_H
#define JOBS_DEBUGSAVEFILE_H

#include "../session.h"
#include "../job.h"
#include "../query_result.h"

class DebugSaveFile
{

    /**
     * @brief Tests saving a sample file.
     */
public:
    static QueryResult execute();
};

#endif  // JOBS_DEBUGSAVEFILE_H
