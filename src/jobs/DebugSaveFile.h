#ifndef JOBS_DEBUGSAVEFILE_H
#define JOBS_DEBUGSAVEFILE_H

#include "../session.h"
#include "../Job.h"
#include "QueryResult.h"

// Tests saving a sample file.
class DebugSaveFile
{
public:
    static QueryResult execute();
};

#endif  // JOBS_DEBUGSAVEFILE_H
