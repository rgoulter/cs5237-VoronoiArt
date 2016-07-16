#include <vector>
#include <utility>

#include "gtest/gtest.h"

#include "delaunay/pointsetio.h"

using std::pair;
using std::string;
using std::vector;
using std::cout;
using std::endl;

using namespace delaunay;



TEST(PointSetIOTest, TrivialCase) {
	const string& input = "[ [1,2], [3,4], [5,6] ]";

	vector< pair<int,int> > actual = parsePointList(input);
	vector< pair<int,int> > expected{ {1,2}, {3,4}, {5,6} };

	EXPECT_EQ(expected, actual);
}



TEST(PointSetIOTest, TrivialVariations) {
	const string& inputPacked = "[[1,2],[3,4],[5,6]]";

	vector< pair<int,int> > actual1 = parsePointList(inputPacked);

	const string& inputSpaces = " [ [ 1 , 2 ] , [ 3 , 4 ] , [ 5 , 6 ] ] ";
	vector< pair<int,int> > actual2 = parsePointList(inputSpaces);

	const string& inputLines = R"([
[1,2],
[3,4],
[5,6]
])";
	vector< pair<int,int> > actual3 = parsePointList(inputLines);


	vector< pair<int,int> > expected{ {1,2}, {3,4}, {5,6} };

	EXPECT_EQ(expected, actual1);
}



TEST(PointSetIOTest, VariousNumbersCase) {
	const string& input = "[ [123,456], [-12 , 0] ]";

	vector< pair<int,int> > actual = parsePointList(input);
	vector< pair<int,int> > expected{ {123,456}, {-12,0} };

	EXPECT_EQ(expected, actual);
}

