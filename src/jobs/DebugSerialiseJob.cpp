#include "./DebugSerialiseJob.h"
#include <sstream>
#include <string>
#include <iomanip>
#include <cassert>

#include "../model/Variant.h"
#include "../model/PropertyValue.h"

std::string testSerialise(const ISerialisable* ser)
{
	std::stringstream log;
	
	Serialiser serialiser;
	ser->serialise(serialiser);

	const char* buffer = serialiser.cdata();
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
}

DebugSerialiseJob::DebugSerialiseJob(ISession* session) : Job(session)
{
}

QueryResult DebugSerialiseJob::execute()
{
	Variant vs("Sample string");
	Variant vi(5);
	PropertyValue val1(vs, 1.0f);
	PropertyValue val2(vi, 0.2f);

	std::stringstream log;
	log << "======== Testing string variant  ========\n";
	log << "======== String: \"Sample String\" ========\n";
	log << testSerialise(&vs) << "\n";

	log << "======== Testing integer variant ========\n";
	log << "======== Integer: 5              ========\n";
	log << testSerialise(&vi) << "\n";

	log << "======== Testing Property Value 1  ========\n";
	log << testSerialise(&val1) << "\n";

	log << "======== Testing Property Value 2  ========\n";
	log << testSerialise(&val2) << "\n";

	// Testing unserialising!
	Serialiser s;
	vs.serialise(s);
	Variant vsUnserialised = Variant::unserialise(s.cdata());
	log << "Unserialised variant string:\n"
	    << vsUnserialised.getString() << "\n";

	s.clear();
	vi.serialise(s);
	Variant viUnserialised = Variant::unserialise(s.cdata());
	log << "Unserialised variant integer:\n"
	    << viUnserialised.getInteger() << "\n";

	s.clear();
	val1.serialise(s);
	PropertyValue val1Uns = PropertyValue::unserialise(s.cdata());
	log << "Unserialised PropertyValue 1:\n("
	    << val1Uns.value().getString() << ","
			<< val1Uns.confidence() << ")\n";

	s.clear();
	val2.serialise(s);
	PropertyValue val2Uns = PropertyValue::unserialise(s.cdata());
	log << "Unserialised PropertyValue 2:\n("
	    << val2Uns.value().getInteger() << ","
			<< val2Uns.confidence() << ")\n";

	QueryResult result;
	result.setValue("type", "string");
	result.setValue(std::string("response"), log.str());
	return result;
}
