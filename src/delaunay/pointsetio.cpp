#include "delaunay/pointsetio.h"

#include <fstream>
#include <sstream>

using std::pair;
using std::string;
using std::vector;
using std::make_pair;

namespace delaunay {


void skipSpaces(string::const_iterator& p, const string::const_iterator& end) {
	while (p != end &&
	       (*p == ' ' ||
	        *p == '\t' ||
	        *p == '\r' ||
	        *p == '\n')) {
		++p;
	}
}


int parseInt(string::const_iterator& p, const string::const_iterator& end) {
	skipSpaces(p, end);

	if (p == end) {
		// some error occurred, but no way to report.
		return 0;
	}

	// Look for a `-` or `+`
	int mult = 1;
	switch (*p) {
	case '-':
		mult = -1;
		++p;
		break;
	case '+':
		mult = 1;
		++p;
	default:
		break;
	}

	// Parse remaining digits
	int number = 0;
	while (p != end &&
	       '0' <= *p && *p <= '9') {
		int digit = (int) (*p - '0');
		number = (10 * number) + digit;
		++p;
	}

	return mult * number;
}



// Parse "[3,4]", "  [ 3 , 4 ]" etc.
pair<int,int> parsePoint(string::const_iterator& p, const string::const_iterator& end) {
	skipSpaces(p, end);

	if (p == end) {
		return make_pair(0,0);  // error
	}

	if (*p == '[') {
		++p;
	} else {
		return make_pair(0,0);  // error
	}

	int x = parseInt(p, end);  // parse x

	skipSpaces(p, end);

	if (p == end) {
		return make_pair(0,0);  // error
	}

	if (*p == ',') {
		++p;
	} else {
		return make_pair(0,0);  // error
	}

	int y = parseInt(p, end);  // parse y

	skipSpaces(p, end);

	if (p == end) {
		return make_pair(0,0);  // error
	}

	if (*p == ']') {
		++p;
	} else {
		return make_pair(0,0);  // error
	}

	return make_pair(x, y);
}



vector< pair<int,int> > parsePointList(string::const_iterator& p, const string::const_iterator& end) {
	vector< pair<int,int> > result;

	skipSpaces(p, end);
	if (p == end) {
		return vector< pair<int,int> >();  // error
	}

	if (*p == '[') {
		++p;
	} else {
		return vector< pair<int,int> >();  // error
	}

	while(p != end) {
		const pair<int,int>& pt = parsePoint(p, end);
		result.push_back(pt);

		skipSpaces(p, end);
		if (p == end) {
			return vector< pair<int,int> >();  // error
		}

		if (*p == ',') {
			++p;
		} else if (*p == ']') {
			++p;
			skipSpaces(p, end);  // read to end
		} else {
			return vector< pair<int,int> >();  // error
		}
	}

	return result;
}



vector< pair<int,int> > parsePointList(const string& s) {
	string::const_iterator p = s.begin();
	string::const_iterator end = s.end();

	return parsePointList(p, end);
}



vector< pair<int,int> > parsePointListFile(const string& filename) {
	// from http://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
	// n.b. bad performance for files over 50kb
	std::ifstream t(filename);
	std::stringstream buffer;
	buffer << t.rdbuf();

	return parsePointList(buffer.str());
}

}
