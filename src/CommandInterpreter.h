/*
*	Parses commands, establishes which jobs need doing and then adds jobs to the jobs queue
*/
#include "./session.h"

class CommandInterpreter
{
public:
  static void ProcessCommand(std::shared_ptr<ISession> session, std::string command);
};