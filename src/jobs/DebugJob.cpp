#include "DebugJob.h"
#include <vector>
#include "../Util.h"
#include "DebugSerialise.h"
#include "DebugSaveFile.h"
#include "DebugLoadFile.h"
#include "DebugDumpEntities.h"

DebugJob::DebugJob(std::shared_ptr<ISession> session, const std::string &message) : Job(session, PermType::UserOp)
{
    _message = message;
}

QueryResult DebugJob::executeConst() const
{
    // Split the string by spaces.
    std::vector<std::string> list = util::split(_message, ' ');
    if ( list.size() < 1 )
    {
        QueryResult result;
        result.setErrorCode(QueryResult::ErrorCode::ParseError);
        result.setInfo("No debug parameter specified.");
        return result;
    }

    if ( list[0] == "SERIALISE" )
    {
        return DebugSerialise::execute();
    }
    else if ( list[0] == "SAVEFILE" )
    {
        return DebugSaveFile::execute();
    }
    else if ( list[0] == "LOADFILE" )
    {
        return DebugLoadFile::execute();
    }
    else if ( list[0] == "DUMPENTITIES" )
    {
        return DebugDumpEntities::execute(*this);
    }

    QueryResult result;
    result.setErrorCode(QueryResult::ErrorCode::ParseError);
    result.setInfo(std::string("Debug parameter ") + list[0] + std::string(" not recognised."));
    return result;
}
