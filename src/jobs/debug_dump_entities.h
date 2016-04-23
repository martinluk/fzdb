#ifndef JOBS_DEBUGDUMPENTITIES_H
#define JOBS_DEBUGDUMPENTITIES_H

#include "../session.h"
#include "./debug_job.h"
#include "../query_result.h"

namespace jobs {
/**
* @brief Debugging command. Returns a list of all entities in the database, including all their properties and values.
*/
  class DebugDumpEntities
  {
  public:
      static QueryResult execute(const Debug &j);
  };
}

#endif // DEBUGDUMPENTITIES_H
