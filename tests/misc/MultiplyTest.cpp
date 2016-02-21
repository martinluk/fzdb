#include <limits.h>
#include "gtest/gtest.h"
#include "Multiply.h"

class MultiplyTest : public ::testing::Test {
protected:
	virtual void setUp() {

	}
	virtual void TearDown() {

	}
};

TEST_F(MultiplyTest, twoValuesMultiplied) {
	const int x = 4;
	const int y = 5;
	Multiply multiplication;
	EXPECT_EQ(20, multiplication.twoValuesMultiplied(x, y));
	EXPECT_EQ(6, multiplication.twoValuesMultiplied(2, 3));
}
