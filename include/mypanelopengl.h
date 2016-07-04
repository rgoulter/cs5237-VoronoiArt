#ifndef MYPANELOPENGL_H
#define MYPANELOPENGL_H

#include <string>
#include <vector>

#include <QGLWidget>
#include <QString>

#include "pointSetArray.h"
#include "polypixel.h"

// Imports as part of implementing Fortune's algorithm
#include "Voronoi.h"
#include "VPoint.h"



// As per http://doc.qt.io/qt-5/qglwidget.html
// XXX QGLWidget is legacy, should use QOpenGLWidget instead.
class MyPanelOpenGL : public QGLWidget
{
    Q_OBJECT
public:
	explicit MyPanelOpenGL(QWidget *parent = 0);
	
signals:
	void updateFilename(QString);
	void updateNumPoints(int);
	void updateNumPointsToGenerate(int);

	void imageLoaded();
	void setUsePDF(bool);
	void setVoronoiComputed(bool);

public slots:
	void doOpenImage();

	void doDrawImage();
	void doDrawEdge();
	void doDrawEdgeSharp();
	void doDrawEdgeBlur();
	void doDrawPDF();
	void doDrawEffect();

	void doVoronoiDiagram();
	void doGenerateUniformRandomPoints();
	void doPDF();
	
	void setNumPoints1k();
	void setNumPoints5k();
	void setNumPoints(int);
	
	void doSaveImage();
	void clearAll();
	
	void setShowVoronoiSites(bool b);
	void setShowVoronoiEdges(bool b);

protected:
	// overloaded
	void initializeGL();
	// overloaded
	void resizeGL(int x, int h);
	// overloaded
	void paintGL();
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);

private:
	void insertPoint(LongInt x, LongInt y);

	// TODO Probably could do without these?
	// Position of the image, in canvas space..
	int canvasOffsetX_ = 0;
	int canvasOffsetY_ = 0;


	/// The 'bare-bones' Voronoi regions, represented using `PointSetArray`s.
	std::vector<PointSetArray> voronoiPolygons_;

	/// The `ColoredPolygon`s we use to render the "stain-glass" effect.
	std::vector<ColoredPolygon> renderedPolygons_;

	// DELAUNAY
	PointSetArray inputPointSet_;

	// VORONOI
	/// Vertices for Fortune's algorithm
	vor::Vertices * voronoiVertices_ = new vor::Vertices();
};

#endif // MYPANELOPENGL_H
