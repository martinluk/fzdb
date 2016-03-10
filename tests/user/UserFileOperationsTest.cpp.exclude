#include "gtest/gtest.h"
#include <string>

#include "UserFileOperationsWrapper.h"
#include "user/Permission.h"
#include "user/UserExceptions.h"

/*
 * Making use of UserFileOperationsWrapper to access protected function of UserFileOperations
 */
class UserFileOperationsTest : public ::testing::Test {
protected:
	void setUp() {
	}
	void TearDown() {
	}
};


TEST_F(UserFileOperationsTest, addUser) {
	typedef UserAttributes UserAttr;
	typedef UserFileOperationsWrapper Wrapper;

	Wrapper wrap;
	UserAttr ua; 
	const std::string username = "username";
	ASSERT_THROW(wrap.getUserAttributes(username), UserNotExistException);
	ua.userName = username;
	ua.passwordHash = "passwordhash";
	ua.salt = "salt";
	ua.userGroup = Permission::UserGroup::ADMIN;
	wrap.addUser(ua);
	ASSERT_NO_THROW(wrap.getUserAttributes(username));

}

TEST_F(UserFileOperationsTest, removeUser) {
}
TEST_F(UserFileOperationsTest, updateUser) {
}
