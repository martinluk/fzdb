#ifndef FUZZYDB_JOB
#define FUZZYDB_JOB

#include "ISession.h"
#include "QueryResult.h"
#include <cassert>

//class Database;
#include "./model/Database.h"
#include "./user/Permission.h"

class Job 
{
public:
    typedef Permission::PermissionType PermType;
    Job(std::shared_ptr<ISession> session, PermType permtype) ;
    virtual ~Job() {}
    // Called to execute the functionality of the job.
    QueryResult execute();
    
    // When writing new job:
    // - Implement constOperation() to signify whether or not the job modifies the database.
    // - If the operation is const, override executeConst().
    // - If the operation is not const, override executeNonConst().
    // - When accessing the database, ONLY use the _database protected member, not Singletons!
    // - If you need to modify one of your member variables in executeConst(), declare it as mutable.
    // - Construtor includes PermissionType {ViewDB, ModifyDB, UserOp}
    
    // Executed if a job does not modify data.
    virtual QueryResult executeConst() const 
    {
        assert(false);
        return QueryResult();
    }

    // Executed if a job does (or could) modify data.
    virtual QueryResult executeNonConst()
    {
        assert(false);
        return QueryResult();
    }

    // Re-implement to specify whether the derived job modifies data.
    // This will call the relevant function from above when
    // execute() is called.
    virtual bool constOperation() const = 0;

    std::shared_ptr<ISession> Session() {
        return _session;
    }
protected:
    Permission::PermissionType _permtype;
    std::shared_ptr<ISession> _session;
    Database* _database;
};

#endif
