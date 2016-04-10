#include "gtest/gtest.h"
#include "user/Permission.h"

class PermissionTest : public ::testing::Test {
protected:
	virtual void setUp() {

	}
	virtual void TearDown() {

	}
};

TEST_F(PermissionTest, assertViewDBPermission) {
	Permission::assertViewDBPermission(Permission::UserGroup::GUEST);
	Permission::assertViewDBPermission(Permission::UserGroup::EDITOR);
	Permission::assertViewDBPermission(Permission::UserGroup::ADMIN);
}

TEST_F(PermissionTest, assertModifyDBPermission) {
	try {
		Permission::assertModifyDBPermission(Permission::UserGroup::GUEST);
		FAIL();
	} catch (Permission::UserPermissionException &e) {

	}
	Permission::assertModifyDBPermission(Permission::UserGroup::EDITOR);
    Permission::assertModifyDBPermission(Permission::UserGroup::ADMIN);
}

TEST_F(PermissionTest, assertUserOpPermission) {
	try {
		Permission::assertUserOpPermission(Permission::UserGroup::GUEST);
		FAIL();
	} catch (Permission::UserPermissionException &e) {

	}
	try {
		Permission::assertUserOpPermission(Permission::UserGroup::EDITOR);
		FAIL();
	} catch (Permission::UserPermissionException &e) {

	}
	Permission::assertUserOpPermission(Permission::UserGroup::ADMIN);
}

