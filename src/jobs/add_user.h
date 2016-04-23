#ifndef FUZZY_JOBS_ADD_USER
#define FUZZY_JOBS_ADD_USER

#include "../job.h"

namespace jobs {  
  // Adds a new user to the database. Requires admin privileges.
  class AddUser : public Job {
   public:
    AddUser(std::shared_ptr<ISession> session, const std::string &username, const std::string &password);
          
    virtual bool constOperation() const override { return false; }
    virtual QueryResult executeNonConst() override;
          
   private:
    std::string _username;
    std::string _password;
  };
}
#endif