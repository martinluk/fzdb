#include <limits.h>
#include "gtest/gtest.h"
#include "types\Date.h"

class DateTest : public ::testing::Test {
protected:
	virtual void setUp() {

	}
	virtual void TearDown() {

	}
};

TEST_F(DateTest, encodeAndRecodeDate) {
	model::types::Date::StructuredDate originalDate(1842, 10, 12);
	unsigned long encoded = model::types::Date::encode(originalDate);
	model::types::Date::StructuredDate decoded = model::types::Date::decode(encoded);
	EXPECT_EQ(originalDate.toString(), decoded.toString());
}

TEST_F(DateTest, encodeAndRecodeDateRecord) {
	std::string dateString = "12/10/1842";
	auto encoded = std::make_shared<model::types::Date>(model::types::Date::parseDateString(dateString), model::types::Date::Ordering::EqualTo);
	EXPECT_EQ(dateString, encoded->toString());
}
