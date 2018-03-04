#include <iostream>
#include <string>
#include <vector>

#include "delaunay/longint/li.h"

#include "delaunay/delaunay.h"
#include "delaunay/pointsetio.h"

#include "geometry/polygon.h"

#include "generatepoints.h"

using std::pair;
using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;


/// run with:
/// $ VoronoiCLI <delaunay|voronoi> <--n number|--pf pointfile.txt>
int main(int argc, char *argv[]) {
	vector<string> args;
	for (int i = 0; i < argc; ++i) {
		args.push_back(argv[i]);
	}

	vector< pair<int,int> > inputPoints;

	if (args.size() == 2 + 1) {

		if (args[1] == "--n") {
			// Given n points, generate randomly.
			int n = std::stoi(args[2]);

			inputPoints = generateUniformRandomPoints(1000, 1000, n);

			// Output points
			if (n < 10) {
				cout << "Generated " << n << " random points:" << endl;
				cout << "[" << endl;
				for (const pair<int,int>& pt : inputPoints) {
					cout << "  (" << pt.first << "," << pt.second << ")" << endl;
				}
				cout << "]" << endl;
			}
		} else if (args[1] == "--pf") {
			// Given a file, load from that
			const string& filename = args[2];

			inputPoints = delaunay::parsePointListFile(filename);
		} else {
			cerr << "Bad arguments!" << endl;
		}
	} else {
		// DEFAULT
		inputPoints = generateUniformRandomPoints(1000, 1000, 50);
	}

	// Coerce POINTREP:INTPAIRVEC to POINTREP:POINTSETARRAY
	delaunay::PointSetArray<double> inputPointSet;
	for (pair<int,int> pair : inputPoints) {
		inputPointSet.addPoint(pair.first, pair.second);
	}

	// Generate random points, or load from file?
	vector<Polygon> result = delaunay::runDelaunayAlgorithm(inputPointSet);

	cout << "*** Result: ***" << endl;
	for (const Polygon& polygon : result) {
		cout << "- " << polygon << endl;
	}

	return 0;
}

