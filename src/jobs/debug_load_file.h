#ifndef JOBS_DEBUGLOADFILE_H
#define JOBS_DEBUGLOADFILE_H

#include "../session.h"
#include "../job.h"
#include "../query_result.h"

class DebugLoadFile
/**
 * @brief Tests loading a file that was previously saved using DebugSaveFile. 
 */
{
public:
    static QueryResult execute();
};

#endif  // JOBS_DEBUGLOADFILE_H
