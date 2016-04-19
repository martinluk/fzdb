#include "gtest/gtest.h"
#include <string>

#include "user/user_operation.h"
#include "user/permission.h"
#include "user/user_exceptions.h"
class UserOperationTest : public ::testing::Test {
	public:
		UserOperation uo;
protected:
	void setUp() {
	}
	void TearDown() {
	}
};

/* This is no longer valid - Now accessing user operation requires going through model/database, which is declared as singleton 
TEST_F(UserOperationTest, cacheStaticTest) {
	const std::string userName="testingCacheStaticness";
	UserOperation uon;
	EXPECT_THROW(uo.getUserGroup(userName), UserNotExistException);
	EXPECT_THROW(uon.getUserGroup(userName), UserNotExistException);
	EXPECT_NO_THROW(uo.addUser(userName,std::string("Some_Common_Password"),Permission::UserGroup::EDITOR));
	ASSERT_NO_THROW(uo.getUserGroup(userName));
	ASSERT_NO_THROW(uon.getUserGroup(userName));
}
*/

/* XXX Putting aside as changed function - will uncomment once adjusted
TEST_F(UserOperationTest, loginTest) {
	const std::string userName="testingLogin";
	const std::string password="mypassword";
	//Throws can't login if user does not exist
	ASSERT_THROW(uo.login(userName,password),LoginUnsuccessfulException);
	//Add a testing account
	EXPECT_NO_THROW(uo.addUser(userName,password,Permission::UserGroup::EDITOR));
	//Throws when wrong password
	ASSERT_THROW(uo.login(userName,std::string("Wr0ngPassword")),LoginUnsuccessfulException);
	//Logins in ok when password is correct.
	ASSERT_NO_THROW(uo.login(userName,password));
}
*/

TEST_F(UserOperationTest, addUser) {
	//Assert there does not exist the specific user
	const std::string userName="testingAddUserFunction";
	EXPECT_THROW(uo.getUserGroup(userName), UserNotExistException);
	//Add the user
	ASSERT_NO_THROW(uo.addUser(userName,std::string("Some_Common_Password"),Permission::UserGroup::EDITOR));
	//Expect user exists
	EXPECT_NO_THROW(uo.getUserGroup(userName));
	//Check throws UserAlreadyExistedException if user already exist
	ASSERT_THROW(uo.addUser(userName,std::string("Some_Common_Password"),Permission::UserGroup::EDITOR),UserAlreadyExistException);
	//Testing getUserGroup
	ASSERT_EQ(Permission::UserGroup::EDITOR,uo.getUserGroup(userName));

	//Cannot add user with empty user name.
	ASSERT_DEATH(uo.addUser(std::string(""),std::string("Some_Common_Password"),Permission::UserGroup::EDITOR),"");
}

/* XXX Test not working TODO(Martin) Investigating on seperate branch
TEST_F(DISABLE_UserOperationTest, removeUser) {
	//Assert throw exception when does not exist
	const std::string userName="testingRemoveUserFunction";
	EXPECT_THROW(uo.removeUser(userName), UserNotExistException);
	//Add the user
	EXPECT_NO_THROW(uo.addUser(userName,std::string("my_stupid_password"),Permission::UserGroup::EDITOR));
	//Remove the user
	ASSERT_NO_THROW(uo.removeUser(userName));
	//Asser the user does not exist anymore
	ASSERT_THROW(uo.getUserGroup(userName), UserNotExistException);
}
TEST_F(UserOperationTest, changeUserGroup) {
	//Assert there does not exist the specific user
	const std::string userName="testingChangeUserGroupFunction";
	EXPECT_THROW(uo.removeUser(userName), UserNotExistException);
	//Assert changing an no existing user throws exception
	ASSERT_THROW(uo.changeUserGroup(userName,Permission::UserGroup::EDITOR),UserNotExistException);
	//Add the user
	EXPECT_NO_THROW(uo.addUser(userName,std::string("my_stupid_password"),Permission::UserGroup::EDITOR));
	EXPECT_EQ(uo.getUserGroup(userName),Permission::UserGroup::EDITOR);
	//Change user group
	ASSERT_NO_THROW(uo.changeUserGroup(userName,Permission::UserGroup::ADMIN));
	//Assert change has effect
	ASSERT_EQ(uo.getUserGroup(userName),Permission::UserGroup::ADMIN);

	//Changing user to same group has does not complain.
	ASSERT_NO_THROW(uo.changeUserGroup(userName,Permission::UserGroup::ADMIN));
	ASSERT_EQ(uo.getUserGroup(userName),Permission::UserGroup::ADMIN);
	
	//Not allowed to change user to guest
	ASSERT_DEATH(uo.changeUserGroup(userName,Permission::UserGroup::GUEST),"");
}
*/
TEST_F(UserOperationTest, getUserGroup) {
	//If Empty string, returns guest.
	const std::string blankUserName="";
	ASSERT_EQ(Permission::UserGroup::GUEST,uo.getUserGroup(blankUserName));
	//Querying unknown username throws exception
	const std::string unknownUserName="testingGetUserGroup";
	ASSERT_THROW(uo.getUserGroup(unknownUserName), UserNotExistException);
}

