#include "./echo.h"

using namespace jobs;

Echo::Echo(std::shared_ptr<ISession> session, const std::string &message) : 
    Job(session, PermType::ViewDB) {
    _message = message;
}

QueryResult Echo::executeConst() const
{
    QueryResult result;
    result.setResultDataText(_message);
    return result;
}
