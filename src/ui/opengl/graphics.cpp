#include "ui/opengl/graphics.h"

#include "delaunay/triangle.h"

using std::vector;

using delaunay::DirectedGraph;
using delaunay::LongInt;
using delaunay::PointSetArray;
using delaunay::TriRecord;



void drawAPoint(double x, double y) {
	glPointSize(5);
	glBegin(GL_POINTS);
	glColor3f(0,0,0);
		glVertex2d(x,y);
	glEnd();
	glPointSize(1);
}



void drawALine(double x1, double y1, double x2, double y2) {
	glPointSize(1);
	glBegin(GL_LINE_LOOP);
	glColor3f(0,0,1);
		glVertex2d(x1,y1);
		glVertex2d(x2,y2);
	glEnd();
	glPointSize(1);
}



void drawATriangle(double x1, double y1, double x2, double y2, double x3, double y3) {
	glBegin(GL_POLYGON);
	glColor3f(0,0.5,0);
		glVertex2d(x1,y1);
		glVertex2d(x2,y2);
		glVertex2d(x3,y3);
	glEnd();
}



void drawPointSetArray(const PointSetArray<LongInt>& pointSet) {
	// Draw input points
	for (int i = 1; i <= pointSet.noPt(); i++){
		LongInt px, py;
		pointSet.getPoint(i, px, py);
		drawAPoint(px.doubleValue(), py.doubleValue());
	}
}



void drawDelaunayTriangles(const PointSetArray<LongInt>& pointSet, const vector<TriRecord>& leafNodes) {
	for (size_t i = 0; i < leafNodes.size(); i++){
		TriRecord leafNode = leafNodes[i];

		int pIndex1, pIndex2, pIndex3;
		leafNode.get(pIndex1, pIndex2, pIndex3);

		// Ignore if from the super triangle (i.e. index too large for input set)
		if(pIndex1 > pointSet.noPt() - 3 ||
		   pIndex2 > pointSet.noPt() - 3 ||
		   pIndex3 > pointSet.noPt() - 3) {
			continue;
		}

		// Probably could clean this up..
		LongInt p1x, p1y, p2x, p2y, p3x, p3y;

		pointSet.getPoint(pIndex1, p1x, p1y);
		pointSet.getPoint(pIndex2, p2x, p2y);
		pointSet.getPoint(pIndex3, p3x, p3y);


		// drawATriangle(p1x.doubleValue(), p1y.doubleValue(),
		//               p2x.doubleValue(), p2y.doubleValue(),
		//               p3x.doubleValue(), p3y.doubleValue());

		drawALine(p1x.doubleValue(), p1y.doubleValue(),
		          p2x.doubleValue(), p2y.doubleValue());
		drawALine(p2x.doubleValue(), p2y.doubleValue(),
		          p3x.doubleValue(), p3y.doubleValue());
		drawALine(p3x.doubleValue(), p3y.doubleValue(),
		          p1x.doubleValue(), p1y.doubleValue());
	}
}



// DELAUNAY (it uses voronoiEdges)
void drawVoronoiPolygons(const vector<geometry::Polygon>& voronoiPolys) {
	for (const geometry::Polygon& polygon : voronoiPolys) {
		glColor3f(0,0,1); // blue

		for(const geometry::Edge& edge : polygon.edges()) {
			drawALine(edge.first.x, edge.first.y,
			          edge.second.x, edge.second.y);
		}
	}
}



void drawColoredPolygons(const vector<ColoredPolygon>& renderedPolygons) {
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_SCISSOR_TEST);

	// TODO: Could prob'ly do a re-useable drawPolygon(const Polygon&)
	for (unsigned int i = 0; i < renderedPolygons.size(); i++) {
		ColoredPolygon coloredPoly = renderedPolygons[i];

		glBegin(GL_POLYGON);
		glColor3fv(coloredPoly.rgb); // rgb?

		const geometry::Polygon& poly = coloredPoly.poly;

		for(const geometry::Point<int>& pt : poly.points()) {
			glVertex2d(pt.x, pt.y);
		}

		glEnd();
	}

	glDisable(GL_SCISSOR_TEST);
}



// XXX this should be a method?
void refreshProjection(int width, int height,
                       int& canvas_offsetX, int& canvas_offsetY,
                       ImageData *imData) {
	glViewport (0, 0, (GLsizei) width, (GLsizei) height);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();

	canvas_offsetX = 0;
	canvas_offsetY = 0;

	// If we haven't loaded an image,
	// we don't particularly care what the coord system is.
	if (imData == NULL) {
		// Just some boring thing.
		glOrtho(-1, 1, 1, -1, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		return;
	}

	int imWidth = imData->width();
	int imHeight = imData->height();

	double imageRatio = static_cast<double>(imWidth) / imHeight;
	double windowRatio = static_cast<double>(width) / height;

	if (imageRatio > windowRatio) {
		double ratio = static_cast<double>(width) / height;

		int renderWidth = imWidth;
		int renderHeight = static_cast<int>(imWidth / ratio);

		int delta = (renderHeight - imHeight) / 2;

		glOrtho(0,
		        renderWidth,
		        imHeight + delta,
		        -delta,
		        -1,
		        1);

		// Scissor test to draw stuff only within the image
		// (Use this for the voronoi-diagram-colors
		int scissorDelta = delta * height / renderHeight;
		canvas_offsetY = scissorDelta;
		glScissor(0, scissorDelta, width, height - (2 * scissorDelta));
	} else {
		double ratio = static_cast<double>(width) / height;

		int renderWidth = static_cast<int>(imHeight * ratio);
		int renderHeight = imHeight;

		int delta = (renderWidth - imWidth) / 2;

		glOrtho(-delta,
		        imWidth + delta,
		        renderHeight,
		        0,
		        -1,
		        1);

		// Scissor test to draw stuff only within the image
		// (Use this for the voronoi-diagram-colors
		int scissorDelta = delta * width / renderWidth;
		canvas_offsetX = scissorDelta;
		glScissor(scissorDelta, 0, width - (2 * scissorDelta), height);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
