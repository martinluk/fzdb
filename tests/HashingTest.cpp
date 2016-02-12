#include <limits.h>
#include "gtest/gtest.h"
#include "user/Hashing.h"

class HashingTest : public ::testing::Test {
protected:
	virtual void setUp() {

	}
	virtual void TearDown() {

	}
};

TEST_F(HashingTest, sha1hash) {
	Hashing e;
	std::string hash = e.sha1hash("pwsalt");
	EXPECT_STREQ(hash.c_str(),"2bfa821dd4f3af5adfb5734457839bed7b324819");
}

TEST_F(HashingTest, genSalt) {
	Hashing e;
	std::string hash = e.genSalt();
}
