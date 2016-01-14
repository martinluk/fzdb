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
	p.assertViewDBPermission(GUEST);
	p.assertViewDBPermission(EDITOR);
	p.assertViewDBPermission(ADMIN);
}

TEST_F(PermissionTest, assertModifyDBPermission) {
	Permission p;
	try {
		p.assertModifyDBPermission(GUEST);
		FAIL();
	} catch (UserPermissionException &e) {

	}
	p.assertModifyDBPermission(EDITOR);
	try {
		p.assertModifyDBPermission(ADMIN);
		FAIL();
	} catch (UserPermissionException &e) {

	}
}

