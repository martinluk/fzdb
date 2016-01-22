#include "DebugJob.h"
#include <vector>
#include "../Util.h"
#include "DebugSerialise.h"
#include "DebugSaveFile.h"

DebugJob::DebugJob(ISession *session, std::string message) : Job(session)
{
    _message = message;
}

QueryResult DebugJob::execute()
{
    // Split the string by spaces.
    std::vector<std::string> list = util::split(_message, ' ');
    if ( list.size() < 1 )
    {
        QueryResult result;
        result.setValue("type", "string");
        result.setValue(std::string("response"), "No debug parameter specified.");
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

    QueryResult result;
    result.setValue("type", "string");
    result.setValue(std::string("response"), std::string("Debug parameter ") + list[0] + std::string(" not recognised."));
    return result;
}
