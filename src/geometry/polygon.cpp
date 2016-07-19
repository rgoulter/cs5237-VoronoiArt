#include "geometry/polygon.h"

#include <climits>

using std::vector;



namespace geometry {

vector<Point<int>> Polygon::points() const {
	return vector<Point<int>>(points_);
}



vector<Edge> Polygon::edges() const {
	vector<Edge> edges;

	// add edges (0, 1), (1, 2) ... (n-1, 0);
	const unsigned int n = points_.size();

	for (unsigned int idx = 0; idx < n; ++idx) {
		int nextIdx = (idx + 1) % n;

		edges.push_back(Edge(points_[idx], points_[nextIdx]));
	}

	return edges;
}



Rect boundingBox(const Polygon& poly) {
	assert(poly.numPoints() > 0);
	// const Point<int>& pt = poly[0];
	int minX = INT_MAX;  // pt.x;
	int maxX = INT_MIN;  // pt.x;

	int minY = INT_MAX;  // pt.y;
	int maxY = INT_MIN;  // pt.y;

	for (const Point<int>& pt : poly.points()) {
		if (pt.x < minX) {
			minX = pt.x;
		}
		if (pt.x > maxX) {
			maxX = pt.x;
		}
		if (pt.y < minY) {
			minY = pt.y;
		}
		if (pt.y > maxY) {
			maxY = pt.y;
		}
	}

	return Rect({ minX, minY }, { maxX, maxY });
}



// 'inside or touching'.
bool isPointInsideRect(const Rect& rect, const Point<int>& pt) {
	return (rect.left() <= pt.x && pt.x <= rect.right()) &&
	       (rect.top() <= pt.y  && pt.y <= rect.bottom()); // COORD CONFUSION
}



// TODO: This needs clarification
class PolygonClippingIntersection {
public:
	PolygonClippingIntersection(const Point<int>& isectPt,
	                            unsigned int polyNextPt,
	                            unsigned int rectNextPt) :
		intersectionPoint_(isectPt),
		nextPolyPointIdx_(polyNextPt),
		nextRectPointIdx_(rectNextPt) {}

	Point<int> intersectionPoint_;
	unsigned int nextPolyPointIdx_;
	unsigned int nextRectPointIdx_;
	PolygonClippingIntersection *nextIntersection_;
};



Polygon clipPolygonToRectangle(const Polygon& poly, const Rect& rect) {
	// COORD CONFUSION:
	// In this function, it's assumed that y+ points "up",
	// so that Rect 0->1->2->3 is CCW (presumably).
	//
	// But Rect.top() returns y1, which is the smaller y,
	// which is the bottom.
	int x1 = rect.left();
	int y1 = rect.top();
	int x2 = rect.right();
	int y2 = rect.bottom(); // COORD: y2 > y1

	// Algorithm:
	// 1) Find intersection points, "split" the line sections at these points.
	// 2) From some "inside" the polygon point (e.g. the first intersection),
	//     follow the "inside" path to build up our output.

	const Rect& polyBoundingBox = boundingBox(poly);
	int minX = polyBoundingBox.left();
	int minY = polyBoundingBox.top();
	int maxX = polyBoundingBox.right();
	int maxY = polyBoundingBox.bottom(); // COORD: y2 > y1

	if (x1 <= minX && maxX <= x2 &&
	    y1 <= minY && maxY <= y2) {
		// The polygon's bounding box is entirely within the rectangle,
		//  ergo nothing to clip.

		return poly;
	}


	// The order for Rect Poly is
	//   { {x1,y1}, {x2, y1}, {x2, y2}, {x1, y2} },
	// i.e. in CCW order, assuming COORD y+ is UP.
	// TODO: initialiser list would be something cool to try for Polygon
	Polygon rectPoly;
	rectPoly.addPoint(x1, y1);
	rectPoly.addPoint(x2, y1);
	rectPoly.addPoint(x2, y2);
	rectPoly.addPoint(x1, y2);

	vector<Edge> polyEdges = poly.edges();
	const unsigned int N = polyEdges.size();
	vector<Edge> rectEdges = rectPoly.edges();

	vector<PolygonClippingIntersection*> intersections;


	//
	// Find intersections
	//
	for (unsigned int polyEdgeIdx = 0;
	     polyEdgeIdx < N;
	     ++polyEdgeIdx) {
		// This somewhat relies on `poly.edges()[i].first == poly.points()[i]`
		const Edge& polyEdge = polyEdges[polyEdgeIdx];

		for (unsigned int rectEdgeIdx = 0;
		     rectEdgeIdx < 4;
		     ++rectEdgeIdx) {
			const Edge& rectEdge = rectEdges[rectEdgeIdx];

			if (isOverlapping(intersects(polyEdge, rectEdge))) {
				const Point<int>& isectPt =
					findIntersectionPoint(polyEdge, rectEdge);

				int polyNextPtIdx = (polyEdgeIdx + 1) % N;
				int rectNextPtIdx = (rectEdgeIdx + 1) % 4;

				PolygonClippingIntersection *isect =
					new PolygonClippingIntersection(isectPt,
					                                polyNextPtIdx,
					                                rectNextPtIdx);
				intersections.push_back(isect);

				// No need to check remaining edges for intersection with
				// the current edge.
				break;
			}
		}
	}


	// If no intersections, then nothing to clip?
	// (Assuming the poly is wholly inside, rather than outside...).
	if (intersections.empty()) {
		// poly is outside rect
		return Polygon();
	}


	//
	// Set links within I'sects.
	//
	for (unsigned int i = 0; i < intersections.size(); i++) {
		PolygonClippingIntersection *next = intersections[(i + 1) % intersections.size()];
		intersections[i]->nextIntersection_ = next;
	}


	Polygon output;

	// Go through, adding points to output.
	for (unsigned int i = 0; i < intersections.size(); i++) {
		PolygonClippingIntersection& isect = *intersections[i];
		PolygonClippingIntersection& nextIsect = *isect.nextIntersection_;

		const Point<int>& isectPt = isect.intersectionPoint_;
		const int nextPolyPtIdx   = isect.nextPolyPointIdx_;
		const int nextRectPtIdx   = isect.nextRectPointIdx_;

		const Point<int>& polyNextPt = poly[nextPolyPtIdx];
		const Point<int>& rectNextPt = rectPoly[nextRectPtIdx];

		// Add the intersection point itself
		output.addPoint(isectPt);

		// Check whether the next Polygon point after intersection
		// is CCW (INSIDE) or CW (OUTSIDE) the segment
		// from I'sect point to next Rect pt.
		// int ori = orientation({isectPt, rectNextPt}, polyNextPt);
		// bool nextPtInside = ori > 0;

		// `orientation` struggles w/ poly w/ INT_MAX?
		bool nextPtInside = isPointInsideRect(rect, polyNextPt);

		if (nextPtInside) {  // CCW
			// Next point is 'INSIDE'.
			// Take polygon points until next i'sect.

			for (unsigned int ptIdx = nextPolyPtIdx;
			     ptIdx != nextIsect.nextPolyPointIdx_;
			     ptIdx = (ptIdx + 1) % N) {
				const Point<int>& pt = poly[ptIdx];
				output.addPoint(pt);
			}
		} else {  // CW
			// Next point is 'OUTSIDE'.
			// Take rectangle points until next i'sect.

			for (unsigned int ptIdx = nextRectPtIdx;
			     ptIdx != nextIsect.nextRectPointIdx_;
			     ptIdx = (ptIdx + 1) % 4) {
				const Point<int>& pt = rectPoly[ptIdx];
				output.addPoint(pt);
			}
		}
	}


	// Tidy up, delete all isects.
	for (int i = intersections.size() - 1; i >= 0; i--) {
		delete intersections[i];
	}

	return output;
}



int inPoly(const Polygon& poly, const Point<int>& pt) {
	const unsigned int n = poly.numPoints();

	// Find some point outsize of the poly.
	int smallestXPtIdx = 0;

	for (unsigned int i = 1; i < n; ++i) {
		if (poly[i].x < poly[smallestXPtIdx].x) {
			smallestXPtIdx = i;
		}
	}


	// outsidePt must be outside of the polygon.
	const Point<int>& smallestXPt = poly[smallestXPtIdx];
	Point<int> outsidePt(smallestXPt.x - 1, smallestXPt.y);
	const Edge& testEdge = { outsidePt, pt };


	// Now count intersections
	int numIntersections = 0;

	for (const Edge& polyEdge : poly.edges()) {
		if (isOverlapping(intersects(testEdge, polyEdge))) {
			++numIntersections;
		}
	}

	return (numIntersections % 2) == 1;
}

}
