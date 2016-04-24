#ifndef JOBS_DEBUGSERIALISE_H
#define JOBS_DEBUGSERIALISE_H

#include "../session.h"
#include "../job.h"
#include "../query_result.h"

namespace jobs {
/**
* @brief Informally tests entity serialisation and outputs the results for inspection.
*/
class DebugSerialise {
 public:
  static QueryResult execute();
};
}

#endif    // JOBS_DEBUGSERIALISE_H
