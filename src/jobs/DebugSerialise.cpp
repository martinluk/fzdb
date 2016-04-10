#include "./DebugSerialise.h"

#include <sstream>
#include <string>
#include <iomanip>
#include <cassert>
#include "spdlog/spdlog.h"

#include "../model/TypeSerialiser.h"
#include "../types/Base.h"
#include "../types/Int.h"
#include "../types/String.h"
#include "../types/EntityRef.h"
#include "../model/Entity.h"
#include "../model/EntitySerialiser.h"

std::string outputSerialiserData(const Serialiser &serialiser)
{
    std::stringstream log;

    const char* buffer = serialiser.cbegin();
    std::size_t length = serialiser.size();

    log << "Serialisation wrote " << length << " bytes.\nBytes written:\n";

    // i progresses in multiples of 8.
    unsigned int total = 0;
    unsigned int line = 1;
    for (int i = 0; i < length; i += 8)
    {
            if ( i > 0 )
                log << "\n";

            log.unsetf(std::ios_base::right);
            log.setf(std::ios_base::left);
            log << std::setfill (' ') << std::setw(6) << (std::string("[") + std::to_string(line) + std::string("]"));

            log.unsetf(std::ios_base::left);
            log.setf(std::ios_base::right);
            log << std::setw(0) << " ";

            log << std::hex << std::setfill ('0') << std::setw(2);

            // j selects the characters in each batch of 8.
            for(int j = 0; j < 8 && i+j < length; j++)
            {
                    if ( j > 0 )
                            log << " ";

                    // If we don't cast to unsigned char, the int will sometimes end up
                    // with a negative sign and spam 'ffffff', which we don't want.
                    int num = static_cast<unsigned char>(buffer[i+j]);
                    log << std::setfill ('0') << std::setw(2) << std::hex << num;
                    total++;

                    if ( i+j == length-1 )
                    {
                        for (int space = 0; space < 8 - j - 1; space++)
                        {
                            log << "   ";
                        }
                    }
            }

            log << "\t";

            for(int j = 0; j < 8 && i+j < length; j++)
            {
                    if ( buffer[i+j] < 32 || buffer[i+j] > 126 )
                            log << '.';
                    else
                            log << buffer[i+j];

                    if ( i+j == length-1 )
                    {
                        for (int space = 0; space < 8 - j - 1; space++)
                        {
                            log << " ";
                        }
                    }
            }

            log << " | ";

            log.unsetf(std::ios_base::right);
            log.setf(std::ios_base::left);
            log.setf(std::ios_base::showbase);

            log << std::setfill (' ') << std::setw(6) << std::hex << total;
            log.unsetf(std::ios_base::showbase);
            log << " (" << std::dec << total << ")";

            log.unsetf(std::ios_base::left);
            log.setf(std::ios_base::right);

            line++;
    }

    return log.str();
}

std::string testSerialise(const std::shared_ptr<model::types::Base> ser)
{
        std::stringstream log;
        Serialiser serialiser;

        model::types::TypeSerialiser typeSerialiser(ser);
        typeSerialiser.serialise(serialiser);

        log << outputSerialiserData(serialiser);

        return log.str();
}

std::string testSerialise(const std::shared_ptr<Entity> ent)
{
    std::stringstream log;
    Serialiser serialiser;

    EntitySerialiser entSerialiser(ent);
    entSerialiser.serialise(serialiser);

    log << outputSerialiserData(serialiser);

    return log.str();
}

std::string printEntity(const std::shared_ptr<Entity> ent)
{
    return std::string("Entity(")
            + std::to_string(ent->getType()) + std::string(", ")
            + std::to_string(ent->getHandle()) + std::string(", ")
            + std::to_string(ent->propertyCount())
            + std::string(")");
}

std::string printEntityProperty(const std::shared_ptr<EntityProperty> prop)
{
    std::string str = std::string("EntityProperty(") + std::to_string(prop->key());

    for ( int i = 0; i < prop->count(); i++ )
    {
        str += std::string(",\n    ") + prop->baseValue(i)->logString();
    }

    str += std::string("\n)");
    return str;
}

QueryResult DebugSerialise::execute()
{
    using namespace model::types;

    std::stringstream log;
    Serialiser serialiser;

    std::shared_ptr<Base> tBase = std::make_shared<Base>(53, 0, std::string());
    std::shared_ptr<Int> tInt = std::make_shared<Int>(1337, 0, (unsigned char)72);
    std::shared_ptr<String> tString = std::make_shared<String>(std::string("Body of Baywatch, face of Crimewatch"), 0, 26);
    std::shared_ptr<EntityRef> tEntRef = std::make_shared<EntityRef>((EHandle_t)1234, 0, 99);

    log << "Testing serialisation of Base type.\n";
    log << testSerialise(tBase) << "\n";

    {
        serialiser.clear();
        TypeSerialiser tser(tBase);
        tser.serialise(serialiser);
        std::shared_ptr<Base> newBase = TypeSerialiser::unserialise(serialiser.cbegin());
        log << "Unserialised Base: " << newBase->logString() << "\n";
    }

    log << "\n";

    log << "Testing serialisation of Int type.\n";
    log << testSerialise(tInt) << "\n";

    {
        serialiser.clear();
        TypeSerialiser tser(tInt);
        tser.serialise(serialiser);
        std::shared_ptr<Base> newBase = TypeSerialiser::unserialise(serialiser.cbegin());
        log << "Unserialised Int: " << newBase->logString() << "\n";
    }

    log << "\n";

    log << "Testing serialisation of String type.\n";
    log << testSerialise(tString) << "\n";

    {
        serialiser.clear();
        TypeSerialiser tser(tString);
        tser.serialise(serialiser);
        std::shared_ptr<Base> newBase = TypeSerialiser::unserialise(serialiser.cbegin());
        log << "Unserialised String: " << newBase->logString() << "\n";
    }

    log << "\n";

    log << "Testing serialisation of EntityRef type.\n";
    log << testSerialise(tEntRef) << "\n";

    {
        serialiser.clear();
        TypeSerialiser tser(tEntRef);
        tser.serialise(serialiser);
        std::shared_ptr<Base> newBase = TypeSerialiser::unserialise(serialiser.cbegin());
        log << "Unserialised EntityRef: " << newBase->logString() << "\n";
    }

    log << "\n";

    std::shared_ptr<Entity> ent = std::make_shared<Entity>(12345, 67890);

    {
        std::vector<std::shared_ptr<String>> values;
        values.push_back(std::make_shared<String>("This", 100));
        values.push_back(std::make_shared<String>("Is", 90));
        values.push_back(std::make_shared<String>("A", 80));
        values.push_back(std::make_shared<String>("Test", 72));
        //ent->insertProperty<String>(new EntityProperty<String>(1, values));
    }

    {
        std::vector<std::shared_ptr<Int>> values;
        values.push_back(std::make_shared<Int>(1337, 100));
        values.push_back(std::make_shared<Int>(420, 90));
        values.push_back(std::make_shared<Int>(8008, 80));
        values.push_back(std::make_shared<Int>(2112, 72));
        //ent->insertProperty<Int>(new EntityProperty<Int>(2, values));
    }

    log << "Testing serialisation of Entity.\n";
    log << testSerialise(ent) << "\n";

    {
        serialiser.clear();
        EntitySerialiser eSer(ent);
        eSer.serialise(serialiser);
        std::shared_ptr<Entity> newEnt = eSer.unserialise(serialiser.begin(), serialiser.size());
        log << "Unserialised entity: " << printEntity(newEnt) << "\nProperties:\n";

        const std::map<unsigned int, std::shared_ptr<EntityProperty>> &propTable = newEnt->properties();
        bool begin = true;
        for ( auto it = propTable.cbegin(); it != propTable.cend(); ++it )
        {
            if ( !begin ) log << ",\n";
            log << printEntityProperty(it->second);
            begin = false;
        }

        log << "\n";

        //With shared_ptr newEnt will be deleted automatically when it falls out of scope
        //delete newEnt;
    }
    
    QueryResult result;
    result.setResultDataText(log.str());
    return result;
}
