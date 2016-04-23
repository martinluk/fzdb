#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <vector>
#include <string>

#include "types/int.h"
#include "types/string.h"
#include "types/date.h"
#include "types/entity_ref.h"
#include "types/subtype.h"
#include "model/entity.h"
#include "model/entity_manager.h"
#include "model/database.h"

#include "model/type_serialiser.h"
#include "model/serialiser.h"
#include "model/entity_serialiser.h"
#include "model/graph_serialiser.h"

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

    std::shared_ptr<EntityProperty> propInt = std::make_shared<EntityProperty>(EntityProperty::Type::FUZZY, idBase++, model::types::SubType::Int32);
	std::shared_ptr<EntityProperty> propString = std::make_shared<EntityProperty>(EntityProperty::Type::FUZZY, idBase++, model::types::SubType::String);
	std::shared_ptr<EntityProperty> propDate = std::make_shared<EntityProperty>(EntityProperty::Type::FUZZY, idBase++, model::types::SubType::Date);
	std::shared_ptr<EntityProperty> propEntityRef = std::make_shared<EntityProperty>(EntityProperty::Type::FUZZY, idBase++, model::types::SubType::EntityRef);

	auto intRecord1 = std::make_shared<Int>(1234, 42, 98, "Integer comment");
	auto intRecord2 = std::make_shared<Int>(5678, 24, 89, "Integer comment");
	intRecord1->Init(98);
	intRecord2->Init(89);
    propInt->append(intRecord1);
    propInt->append(intRecord2);

	auto stringRecord1 = std::make_shared<String>("Serialise this! @#!@$@%", 23, 1, "String comment");
	auto stringRecord2 = std::make_shared<String>("Mew Purrymore", 32, 2, "I for one welcome our new robot overlords.");
	stringRecord1->Init(1);
	stringRecord2->Init(2);
    propString->append(stringRecord1);
    propString->append(stringRecord2);

	auto dateRecord1 = std::make_shared<Date>(Date::StructuredDate(1150, 3, 21), Date::Ordering::After);
	auto dateRecord2 = std::make_shared<Date>(Date::StructuredDate(1210, 4, 4), Date::Ordering::Before);
	dateRecord1->Init(1);
	dateRecord2->Init(1);
    propDate->append(dateRecord1);
    propDate->append(dateRecord2);

	auto entityRecord1 = std::make_shared<EntityRef>(1);
	auto entityRecord2 = std::make_shared<EntityRef>(5674564);
	entityRecord1->Init(100);
	entityRecord2->Init(80);
    propEntityRef->append(entityRecord1);
    propEntityRef->append(entityRecord2);

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

	auto intRecord = BasePointer(new Int(1234, 42, 98, "Integer comment"));
	auto stringRecord = BasePointer(new String("Serialise this! @#!@$@%", 32, 1, "String comment"));
	auto dateRecord = BasePointer(new Date(Date::StructuredDate(1150, 3, 21), Date::Ordering::EqualTo));
	auto entityRefRecord = BasePointer(new EntityRef((EHandle_t)1));

	intRecord->Init(98);
	stringRecord->Init(1);
	dateRecord->Init(1);
	entityRefRecord->Init(100);

    testSerialisation(intRecord);
    testSerialisation(stringRecord);
    testSerialisation(dateRecord);
    testSerialisation(entityRefRecord);
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
	Database database("test.dat");
    EntityManager manager(&database);
	manager.clearAll();
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
	Database database("test.dat");
    EntityManager manager(&database);
	manager.clearAll();
    std::shared_ptr<Entity> ent = manager.createEntity("newType");
    createSampleEntity(ent, 7);
    std::shared_ptr<Entity> ent2 = manager.createEntity("newType");
    manager.linkEntities(ent->getHandle(), ent2->getHandle());

    manager.saveToFile("serialisationTest.bin");

    EntityManager manager2(NULL);
    manager2.loadFromFile("serialisationTest.bin");

    EXPECT_EQ(true, manager.memberwiseEqual(manager2));
}
