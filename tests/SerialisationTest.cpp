#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <vector>
#include <string>

#include "model/types/Int.h"
#include "model/types/String.h"
#include "model/types/Date.h"
#include "model/types/EntityRef.h"
#include "model/types/SubType.h"

#include "model/TypeSerialiser.h"
#include "model/Serialiser.h"

class SerialisationTest : public ::testing::Test
{
protected:
    virtual void setUp() {

    }
    virtual void TearDown() {

    }
};

template<typename T>
void testSerialisation(const BasePointer &typePtr)
{
    Serialiser ser;
    model::types::TypeSerialiser tSer(typePtr);
    tSer.serialise(ser);
    BasePointer unserialised = tSer.unserialise(ser.begin());

    // For debugging:
    char* serialisedData = ser.begin();
    const T* preSerialise = (const T*)typePtr.get();
    const T* postSerialise = (const T*)unserialised.get();

    EXPECT_EQ(true, unserialised->memberwiseEqual(typePtr.get()));
}

// Test that values of different types serialise and unserialise correctly.
TEST_F(SerialisationTest, testSerialiseValues)
{
    using namespace model::types;

//    BasePointer tInt(new Int(1234, 42, 98, "Integer comment"));
//    BasePointer tString(new String("Serialise this! @#!@$@%", 32, 1, "String comment"));
//    BasePointer tDate(new Date(Date::StructuredDate(1150, 3, 21), 1, Date::Ordering::EqualTo, 12, "Date comment"));
//    BasePointer tEntityRef(new EntityRef((EHandle_t)1, 123785, 99, "Entity ref comment"));

    testSerialisation<Int>(BasePointer(new Int(1234, 42, 98, "Integer comment")));
    testSerialisation<String>(BasePointer(new String("Serialise this! @#!@$@%", 32, 1, "String comment")));
    testSerialisation<Date>(BasePointer(new Date(Date::StructuredDate(1150, 3, 21), 1, Date::Ordering::EqualTo, 12, "Date comment")));
    testSerialisation<EntityRef>(BasePointer(new EntityRef((EHandle_t)1, 123785, 99, "Entity ref comment")));

    EXPECT_EQ(true, false);
}

// Test that properties with different numbers of values serialise and unserialise correctly.
TEST_F(SerialisationTest, testSerialiseProperties)
{

    EXPECT_EQ(true, false);
}

// Test that entities with different numbers of properties serialise and unserialise correctly.
TEST_F(SerialisationTest, testSerialiseEntities)
{

    EXPECT_EQ(true, false);
}

// Test that entity manager with different numbers of entities serialises and unserialises correctly.
TEST_F(SerialisationTest, testSerialiseEntityManager)
{

    EXPECT_EQ(true, false);
}

// Check that files are written to and read from disk correctly.
TEST_F(SerialisationTest, testWriteReadFiles)
{

    EXPECT_EQ(true, false);
}
