#include <limits.h>
#include "gtest/gtest.h"
#include "Cubic.h"

class CubicTest : public ::testing::Test {
protected:
	virtual void setUp() {

	}
	virtual void TearDown() {

	}
};

TEST_F(CubicTest, cube) {
	const int x = 4;
	Cubic cubic;
	EXPECT_EQ(64, cubic.cube(x));
	EXPECT_EQ(125, cubic.cube(5));
}