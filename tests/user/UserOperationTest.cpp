#include "gtest/gtest.h"
#include "user/UserOperation.h"
class UserOperationTest : public ::testing::Test {
protected:
	void setUp() {
		UserFileOperations::initialize();
	}
	void TearDown() {
		//Reset the user file TODO
		//UserFileOperations::initialize();
	}
};

TEST_F(UserOperationTest, addUser) {
	//Assert there does not exist the specific user

	try {
		const std::string userName="addUserTest";
		UserGroup ug = UserOperation::getUserGroup(userName);
		FAIL();	//Should definitely have exception thrown here.
	} catch (UserNotExistException ex) {
		//Do nothing.
		return;
	}
	//Add the user

	//Assert user exists
}

TEST_F(UserOperationTest, removeUser) {
	//Assert there does not exist the specific user

	//Add the user

	//Assert user exists

	//Remove the user

	//Asser the user does not exist anymore
}
TEST_F(UserOperationTest, changeUserGroup) {
	//Assert there does not exist the specific user

	//Add the user

	//Assert user exists

	//Remove the user

	//Asser the user does not exist anymore
}
TEST_F(UserOperationTest, getUserGroup) {
	//TODO
}
