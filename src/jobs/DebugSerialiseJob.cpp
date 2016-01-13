#include "./DebugSerialiseJob.h"

#include <sstream>
#include <string>
#include <iomanip>
#include <cassert>

#include "../model/ISerialisable.h"

std::string testSerialise(const Model::ISerialisable* ser)
{
        std::stringstream log;
	
	Serialiser serialiser;
	ser->serialise(serialiser);

        const char* buffer = serialiser.cbegin();
	std::size_t length = serialiser.size();

	log << "Serialisation wrote " << length << " bytes.\nBytes written:\n";
	log << std::hex << std::setfill ('0') << std::setw(2);
	
	// i progresses in multiples of 8.
	for (int i = 0; i < length; i += 8)
	{
		// j selects the characters in each batch of 8.
		for(int j = 0; j < 8 && i+j < length; j++)
		{
			if ( j > 0 )
				log << " ";

			log << std::setfill ('0') << std::setw(2) << std::hex << static_cast<int>(buffer[i+j]);
		}

		log << "\t";

		for(int j = 0; j < 8 && i+j < length; j++)
		{
			if ( buffer[i+j] < 32 || buffer[i+j] > 126 )
				log << '.';
			else
				log << buffer[i+j];
		}

		log << "\n";
	}

        return log.str();
	return "";
}

DebugSerialiseJob::DebugSerialiseJob(ISession* session) : Job(session)
{
}

QueryResult DebugSerialiseJob::execute()
{
    return QueryResult();
}
