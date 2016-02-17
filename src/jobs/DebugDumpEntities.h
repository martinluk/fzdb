#ifndef JOBS_DEBUGDUMPENTITIES_H
#define JOBS_DEBUGDUMPENTITIES_H

#include "../session.h"
#include "DebugJob.h"
#include "../QueryResult.h"

class DebugDumpEntities
{
public:
    static QueryResult execute(const DebugJob &j);
};

#endif // DEBUGDUMPENTITIES_H
