#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <vector>
#include <string>

#include "types/Int.h"
#include "types/String.h"
#include "types/Date.h"
#include "types/EntityRef.h"
#include "types/SubType.h"
#include "model/Entity.h"
#include "model/EntityManager.h"

#include "model/TypeSerialiser.h"
#include "model/Serialiser.h"
#include "model/EntitySerialiser.h"
#include "model/GraphSerialiser.h"

// TODO: Extend these. They probably don't cover all edge cases.

class SerialisationTest : public ::testing::Test
{
protected:
    virtual void setUp() {

    }
    virtual void TearDown() {

    }
};

void testSerialisation(const BasePointer &typePtr)
{
    Serialiser ser;
    model::types::TypeSerialiser tSer(typePtr);
    std::size_t serialisedLength = tSer.serialise(ser);
    std::size_t advance = 0;
    BasePointer unserialised = model::types::TypeSerialiser::unserialise(ser.begin(), &advance);

    EXPECT_EQ(true, unserialised->memberwiseEqual(typePtr.get()));
    EXPECT_EQ(advance, serialisedLength);
}

void createSampleEntity(std::shared_ptr<Entity> &ent, unsigned int idBase)
{
    using namespace model::types;

    EntityProperty* propInt = new EntityProperty(EntityProperty::Type::FUZZY, idBase++, model::types::SubType::Int32);
    EntityProperty* propString = new EntityProperty(EntityProperty::Type::FUZZY, idBase++, model::types::SubType::String);
    EntityProperty* propDate = new EntityProperty(EntityProperty::Type::FUZZY, idBase++, model::types::SubType::Date);
    EntityProperty* propEntityRef = new EntityProperty(EntityProperty::Type::FUZZY, idBase++, model::types::SubType::EntityRef);

    propInt->append(BasePointer(new Int(1234, 42, 98, "Integer comment")));
    propInt->append(BasePointer(new Int(5678, 24, 89, "Integer comment")));

    propString->append(BasePointer(new String("Serialise this! @#!@$@%", 23, 1, "String comment")));
    propString->append(BasePointer(new String("Mew Purrymore", 32, 2, "I for one welcome our new robot overlords.")));

    propDate->append(BasePointer(new Date(Date::StructuredDate(1150, 3, 21), 1, Date::Ordering::After, 12, "Date comment")));
    propDate->append(BasePointer(new Date(Date::StructuredDate(1210, 4, 4), 1, Date::Ordering::Before, 13, "Date comment")));

    propEntityRef->append(BasePointer(new EntityRef((EHandle_t)1)));
    propEntityRef->append(BasePointer(new EntityRef((EHandle_t)5674564)));

    // Entity takes ownership of properties here.
    ent->insertProperty(std::shared_ptr<EntityProperty>(propInt));
    ent->insertProperty(std::shared_ptr<EntityProperty>(propString));
    ent->insertProperty(std::shared_ptr<EntityProperty>(propDate));
    ent->insertProperty(std::shared_ptr<EntityProperty>(propEntityRef));
}

// Test that values of different types serialise and unserialise correctly.
TEST_F(SerialisationTest, testSerialiseValues)
{
    using namespace model::types;

    testSerialisation(BasePointer(new Int(1234, 42, 98, "Integer comment")));
    testSerialisation(BasePointer(new String("Serialise this! @#!@$@%", 32, 1, "String comment")));
    testSerialisation(BasePointer(new Date(Date::StructuredDate(1150, 3, 21), 1, Date::Ordering::EqualTo, 12, "Date comment")));
    testSerialisation(BasePointer(new EntityRef((EHandle_t)1)));
}

// Test that entities with different numbers of properties serialise and unserialise correctly.
TEST_F(SerialisationTest, testSerialiseEntities)
{
    using namespace model::types;

    std::shared_ptr<Entity> ent(new Entity(0, 1));
    createSampleEntity(ent, 7);

    Serialiser serialiser;
    EntitySerialiser entSer(ent);
    entSer.serialise(serialiser);
    std::shared_ptr<Entity> unserialised = EntitySerialiser::unserialise(serialiser.begin(), serialiser.size());

    EXPECT_EQ(true, ent->memberwiseEqual(unserialised.get()));
}

// Test that entity manager with different numbers of entities serialises and unserialises correctly.
TEST_F(SerialisationTest, testSerialiseEntityManager)
{
    EntityManager manager(NULL);
    std::shared_ptr<Entity> ent = manager.createEntity("newType");
    createSampleEntity(ent, 7);
    std::shared_ptr<Entity> ent2 = manager.createEntity("newType");
    manager.linkEntities(ent->getHandle(), ent2->getHandle());

    Serialiser serialiser;
    GraphSerialiser gSer(&manager);
    gSer.serialise(serialiser);

    EntityManager manager2(NULL);
    GraphSerialiser gSer2(&manager2);
    gSer2.unserialise(serialiser.begin(), serialiser.size());

    EXPECT_EQ(true, manager.memberwiseEqual(manager2));
}

// Check that files are written to and read from disk correctly.
TEST_F(SerialisationTest, testWriteReadFiles)
{
    EntityManager manager(NULL);
    std::shared_ptr<Entity> ent = manager.createEntity("newType");
    createSampleEntity(ent, 7);
    std::shared_ptr<Entity> ent2 = manager.createEntity("newType");
    manager.linkEntities(ent->getHandle(), ent2->getHandle());

    manager.saveToFile("serialisationTest.bin");

    EntityManager manager2(NULL);
    manager2.loadFromFile("serialisationTest.bin");

    EXPECT_EQ(true, manager.memberwiseEqual(manager2));
}
