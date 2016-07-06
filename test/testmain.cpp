#include "gtest/gtest.h"



int main(int argc, char* argv[]) {
	// Could skip defining this by linking to GTest's src/gtest_main.cc
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
