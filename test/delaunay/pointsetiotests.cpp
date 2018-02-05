#include <vector>
#include <utility>

#include "catch.hpp"

#include "delaunay/pointsetio.h"

using std::pair;
using std::string;
using std::vector;

using namespace delaunay;

#define SUT_NAME "delaunay/PointSetIO/parsePointList"
#define SUT_TAGS "[delaunay][parsePointList]"



TEST_CASE(SUT_NAME ", Trivial", SUT_TAGS) {
	vector< pair<int,int> > expected{ {1,2}, {3,4}, {5,6} };

	SECTION("Input with typical spacing") {
		const string& input = "[ [1,2], [3,4], [5,6] ]";
		vector< pair<int,int> > actual = parsePointList(input);
		REQUIRE(expected == actual);
	}

	SECTION("Input with packed spacing") {
		const string& inputPacked = "[[1,2],[3,4],[5,6]]";
		vector< pair<int,int> > actual = parsePointList(inputPacked);
		REQUIRE(expected == actual);
	}

	SECTION("Input with spacing everywhere") {
	const string& inputSpaces = " [ [ 1 , 2 ] , [ 3 , 4 ] , [ 5 , 6 ] ] ";
	vector< pair<int,int> > actual2 = parsePointList(inputSpaces);
	}

	SECTION("Multiline input") {
		const string& inputLines = R"([
[1,2],
[3,4],
[5,6]
])";
		vector< pair<int,int> > actual = parsePointList(inputLines);
		REQUIRE(expected == actual);
	}
}



TEST_CASE(SUT_NAME ", Input with various numbers", SUT_TAGS) {
	const string& input = "[ [123,456], [-12 , 0] ]";

	vector< pair<int,int> > actual = parsePointList(input);
	vector< pair<int,int> > expected{ {123,456}, {-12,0} };

	REQUIRE(expected == actual);
}
