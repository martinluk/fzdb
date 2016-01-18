#include "./DebugSerialise.h"

#include <sstream>
#include <string>
#include <iomanip>
#include <cassert>

#include "../model/TypeSerialiser.h"
#include "../model/types/Base.h"
#include "../model/types/Int.h"
#include "../model/types/String.h"
#include "../model/types/EntityRef.h"

std::string testSerialise(const model::types::Base* ser)
{
        std::stringstream log;
        Serialiser serialiser;

        model::types::TypeSerialiser typeSerialiser(ser);
        typeSerialiser.serialise(serialiser);

        const char* buffer = serialiser.cbegin();
        std::size_t length = serialiser.size();

        log << "Serialisation wrote " << length << " bytes.\nBytes written:\n";
        log << std::hex << std::setfill ('0') << std::setw(2);
	
        // i progresses in multiples of 8.
        for (int i = 0; i < length; i += 8)
        {
                if ( i > 0 )
                    log << "\n";

                // j selects the characters in each batch of 8.
                for(int j = 0; j < 8 && i+j < length; j++)
                {
                        if ( j > 0 )
                                log << " ";

                        // If we don't cast to unsigned char, the int will sometimes end up
                        // with a negative sign and spam 'ffffff', which we don't want.
                        int num = static_cast<unsigned char>(buffer[i+j]);
                        log << std::setfill ('0') << std::setw(2) << std::hex << num;
                }

                log << "\t";

                for(int j = 0; j < 8 && i+j < length; j++)
                {
                        if ( buffer[i+j] < 32 || buffer[i+j] > 126 )
                                log << '.';
                        else
                                log << buffer[i+j];
                }
        }

        return log.str();
}

QueryResult DebugSerialise::execute()
{
    using namespace model::types;

    std::stringstream log;
    Serialiser serialiser;

    Base tBase(53);
    Int tInt(1337, (unsigned char)72);
    String tString(std::string("Body of Baywatch, face of Crimewatch"), 26);
    EntityRef tEntRef((EHandle_t)1234, 99);

    log << "Testing serialisation of Base type.\n";
    log << testSerialise(&tBase) << "\n";

    {
        serialiser.clear();
        TypeSerialiser tser(&tBase);
        tser.serialise(serialiser);
        Base* newBase = tser.unserialise(serialiser.cbegin());
        log << "Unserialised Base: " << newBase->logString() << "\n";
        delete newBase;
    }

    log << "\n";

    log << "Testing serialisation of Int type.\n";
    log << testSerialise(&tInt) << "\n";

    {
        serialiser.clear();
        TypeSerialiser tser(&tInt);
        tser.serialise(serialiser);
        Base* newBase = tser.unserialise(serialiser.cbegin());
        log << "Unserialised Int: " << newBase->logString() << "\n";
        delete newBase;
    }

    log << "\n";

    log << "Testing serialisation of String type.\n";
    log << testSerialise(&tString) << "\n";

    {
        serialiser.clear();
        TypeSerialiser tser(&tString);
        tser.serialise(serialiser);
        Base* newBase = tser.unserialise(serialiser.cbegin());
        log << "Unserialised String: " << newBase->logString() << "\n";
        delete newBase;
    }

    log << "\n";

    log << "Testing serialisation of EntityRef type.\n";
    log << testSerialise(&tEntRef) << "\n";

    {
        serialiser.clear();
        TypeSerialiser tser(&tEntRef);
        tser.serialise(serialiser);
        Base* newBase = tser.unserialise(serialiser.cbegin());
        log << "Unserialised EntityRef: " << newBase->logString() << "\n";
        delete newBase;
    }

    QueryResult result;
    result.setValue("type", "string");
    result.setValue(std::string("response"), log.str());
    return result;
}
