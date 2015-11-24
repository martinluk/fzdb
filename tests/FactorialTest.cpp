#include <limits.h>
#include "gtest/gtest.h"
#include "Factorial.h"

class FactorialTest : public ::testing::Test {
protected:
	virtual void setUp() {

	}
	virtual void TearDown() {

	}
};

TEST_F(FactorialTest, fact) {
	const int x = 4;
	Factorial factorial;
	EXPECT_EQ(24, factorial.fact(x));
	EXPECT_EQ(720, factorial.fact(6));
}
