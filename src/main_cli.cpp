#include <iostream>
#include <string>
#include <vector>

#include "delaunay/delaunay.h"
#include "delaunay/pointsetio.h"

#include "voronoi/voronoi.h"

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

	// Delaunay | Voronoi
	enum VoronoiAlgorithm {
		kDelaunayAlgorithm,
		kVoronoiAlgorithm
	};

	VoronoiAlgorithm selectedAlgorithm;
	vector< pair<int,int> > inputPoints;

	if (args.size() == 3 + 1) {
		if (args[1] == "voronoi") {
			selectedAlgorithm = kVoronoiAlgorithm;
		} else {
			selectedAlgorithm = kDelaunayAlgorithm;
		}

		if (args[2] == "--n") {
			// Given n points, generate randomly.
			int n = std::stoi(args[3]);

			inputPoints = generateUniformRandomPoints(1000, 1000, n);
		} else if (args[2] == "--pf") {
			// Given a file, load from that
			const string& filename = args[3];

			inputPoints = delaunay::parsePointListFile(filename);
		} else {
			cerr << "Bad arguments!" << endl;
		}
	} else {
		// DEFAULT
		selectedAlgorithm = kDelaunayAlgorithm;
		inputPoints = generateUniformRandomPoints(1000, 1000, 50);
	}

	// Coerce POINTREP:INTPAIRVEC to POINTREP:POINTSETARRAY
	delaunay::PointSetArray inputPointSet;
	for (pair<int,int> pair : inputPoints) {
		inputPointSet.addPoint(pair.first, pair.second);
	}

	// Generate random points, or load from file?
	if (selectedAlgorithm == kDelaunayAlgorithm) {  // Delaunay
		delaunay::runDelaunayAlgorithm(inputPointSet);
	} else {  // Voronoi/Fortune's
		voronoi::runVoronoiAlgorithm(inputPointSet);
	}

	return 0;
}

