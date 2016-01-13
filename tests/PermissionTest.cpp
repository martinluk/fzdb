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
	//p.assertViewDBPermission(GUEST); TODO Link enum to test case

}
