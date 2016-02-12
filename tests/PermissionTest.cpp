#include <limits.h>
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
	Permission p;
	p.assertViewDBPermission(UserGroup::GUEST);
	p.assertViewDBPermission(UserGroup::EDITOR);
	p.assertViewDBPermission(UserGroup::ADMIN);
}

TEST_F(PermissionTest, assertModifyDBPermission) {
	Permission p;
	try {
		p.assertModifyDBPermission(UserGroup::GUEST);
		FAIL();
	} catch (UserPermissionException &e) {

	}
	p.assertModifyDBPermission(UserGroup::EDITOR);
	try {
		p.assertModifyDBPermission(UserGroup::ADMIN);
		FAIL();
	} catch (UserPermissionException &e) {

	}
}

TEST_F(PermissionTest, assertUserOpPermission) {
	Permission p;
	try {
		p.assertUserOpPermission(UserGroup::GUEST);
		FAIL();
	} catch (UserPermissionException &e) {

	}
	try {
		p.assertUserOpPermission(UserGroup::EDITOR);
		FAIL();
	} catch (UserPermissionException &e) {

	}
	p.assertUserOpPermission(UserGroup::ADMIN);
}

