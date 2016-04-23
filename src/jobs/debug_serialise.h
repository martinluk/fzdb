#ifndef JOBS_DEBUGSERIALISE_H
#define JOBS_DEBUGSERIALISE_H

#include "../session.h"
#include "../job.h"
#include "../query_result.h"

class DebugSerialise
{


    /**
     * @brief Informally tests entity serialisation and outputs the results for inspection. 
     */
public:
    static QueryResult execute();
};

#endif    // JOBS_DEBUGSERIALISE_H
