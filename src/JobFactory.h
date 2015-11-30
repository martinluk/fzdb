/*
* Acts as a central point for Job creation - jobs are only created in this class, any new 
* jobs have to be added to this class to be created
*/
#include "Job.h"

class JobFactory
{
public:  
  static void Init();
  static Job* createUnknownJob(ISession* session, std::string const& name);
  static Job* createJob(ISession* session, std::string const& name);
  static Job* createJob(ISession* session, std::string const& name, std::string const& arg1);
};
