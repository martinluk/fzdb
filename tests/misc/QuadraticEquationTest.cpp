#include <limits.h>
#include "gtest/gtest.h"
#include "QuadraticEquation.h"

class QuadraticEquationTest : public ::testing::Test {
protected:
	virtual void setUp() {

	}
	virtual void TearDown() {

	}
};

TEST_F(QuadraticEquationTest, equation) {
	const int a = 4;
	const int x = 2;
	const int b = 3;
	const int y = 5;
	const int c = 10;
	QuadraticEquation quadraticEquation;
	EXPECT_EQ(41, quadraticEquation.equation(a, x, b, y, c));
	EXPECT_EQ(21, quadraticEquation.equation(1, 2, 3, 4, 5));
}