#include "gtest/gtest.h"

TEST(BasicsTest, ASimpleTest) {
 
   float _Base = 4.0f;
   float _Exponent = 2.0f;
 
   float _ExpectedValue = 8.0f;
 
   float _ActualValue = _Base * _Exponent;
 
   EXPECT_FLOAT_EQ(_ExpectedValue, _ActualValue);  // Almost equal
 
}



int main(int argc, char* argv[]) {
	// Could skip defining this by linking to GTest's src/gtest_main.cc
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}