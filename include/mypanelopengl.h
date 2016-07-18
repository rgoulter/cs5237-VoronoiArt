#ifndef MYPANELOPENGL_H
#define MYPANELOPENGL_H

#include <string>
#include <vector>

#include <QGLWidget>
#include <QString>

#include "delaunay/pointsetarray.h"

// Imports as part of implementing Fortune's algorithm
#include "voronoi/voronoi.h"
#include "voronoi/vpoint.h"

#include "geometry/polygon.h"

#include "polypixel.h"
#include "generatepoints.h"



enum ShowImageType {
	IMAGE,
	EDGE_RAW,
	EDGE_SHARP,
	EDGE_BLUR,
	PDF,
	EFFECT,
	NONE
};



enum VoronoiAlgorithm {
	kDelaunayAlgorithm,
	kVoronoiAlgorithm
};



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

	void useDelaunayAlgorithm();
	void useVoronoiAlgorithm();

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
	void insertPoint(delaunay::LongInt x, delaunay::LongInt y);
	bool hasLoadedImage() { return imData_ != NULL; }

	// TODO Probably could do without these?
	// Position of the image, in canvas space..
	int canvasOffsetX_ = 0;
	int canvasOffsetY_ = 0;

	std::string loadedImageFilename_ = "";
	ImageData *imData_ = NULL;
	PDFTextures pdfTextures_;

	// OpenGL stuff
	ShowImageType currentRenderType_ = NONE;
	bool showVoronoiSites_ = true;
	bool showVoronoiEdges_ = false;


	int numPDFPoints_ = 75;

	/// The 'bare-bones' Voronoi regions, represented using `PointSetArray`s.
	std::vector<geometry::Polygon> voronoiPolygons_;

	/// The `ColoredPolygon`s we use to render the "stain-glass" effect.
	std::vector<ColoredPolygon> renderedPolygons_;

	// DELAUNAY
    delaunay::PointSetArray inputPointSet_;

	VoronoiAlgorithm algorithm_ = kVoronoiAlgorithm;
};

#endif // MYPANELOPENGL_H
