#include "gtest/gtest.h"
#include <string>

#include "UserFileOperationsWrapper.h"
#include "user/Permission.h"
#include "user/UserExceptions.h"

/*
 * Making use of UserFileOperationsWrapper to access protected function of UserFileOperations
 */
class UserFileOperationsTest : public ::testing::Test {
	public:
		UserFileOperationsWrapper uo;
protected:
	void setUp() {
	}
	void TearDown() {
	}
};

TEST_F(UserFileOperationsTest, addUser) {
}

TEST_F(UserFileOperationsTest, removeUser) {
}
TEST_F(UserFileOperationsTest, updateUser) {
}
TEST_F(UserFileOperationsTest, getUserAttributes) {
}
