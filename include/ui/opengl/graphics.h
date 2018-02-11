#ifndef UI_OPENGL_GRAPHICSH
#define UI_OPENGL_GRAPHICSH

#include <vector>

#include "delaunay/longint/li.h"

#include "delaunay/pointsetarray.h"

#include "geometry/polygon.h"

#include "imagedata.h"
#include "polypixel.h"

#include "platform.h"

void drawAPoint(double x, double y);

void drawALine(double x1, double y1, double x2, double y2);

void drawATriangle(double x1,double y1, double x2, double y2, double x3, double y3);

void drawPointSetArray(const delaunay::PointSetArray<delaunay::LongInt>& pointSet);

void drawVoronoiPolygons(std::vector<geometry::Polygon>& voronoiPolys);

void drawColoredPolygons(const std::vector<ColoredPolygon>& renderedPolygons);

void refreshProjection(int width, int height,
                       int& canvas_offsetX, int& canvas_offsetY,
                       ImageData *imData);

#endif