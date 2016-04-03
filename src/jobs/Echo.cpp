#include "Echo.h"

EchoJob::EchoJob(std::shared_ptr<ISession> session, const std::string &message) : 
    Job(session, PermType::UserOp) {
    _message = message;
}

QueryResult EchoJob::executeConst() const
{
    QueryResult result;
    result.setResultDataText(_message);
    return result;
}
