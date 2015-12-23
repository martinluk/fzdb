/*
* Acts as a central point for Job creation - jobs are only created in this class, any new 
* jobs have to be added to this class to be created
*/
#include "Job.h"
#include <vector>
#include "./ISession.h"

class JobFactory
{
public:  
  static void Init();

	// TODO: Deprecate these.
  static Job* createUnknownJob(ISession* session, std::string const& name);
  static Job* createJob(ISession* session, std::string const& name);
  static Job* createJob(ISession* session, std::string const& name, std::string const& arg1);

	// PROPOSED: Pass a std::vector of string arguments? Then we only need one function.
	// The name of the job is always arguemnt 0, and arguments 1+ are passed in as required.
	static Job* createJob(ISession* session, const std::vector<std::string> &args);

};
