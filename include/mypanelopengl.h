#ifndef MYPANELOPENGL_H
#define MYPANELOPENGL_H

#include <string>
#include <vector>

#include <QGLWidget>
#include <QString>

#include "delaunay/longint/li.h"

#include "delaunay/pointsetarray.h"

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



// As per http://doc.qt.io/qt-5/qglwidget.html
// XXX QGLWidget is legacy, should use QOpenGLWidget instead.
class MyPanelOpenGL : public QGLWidget
{
    Q_OBJECT
public:
	explicit MyPanelOpenGL(QWidget *parent = 0);

signals:
	/// Provides a way to update the *textfield* in mainqt
	void updateFilename(QString);
	/// Updates UI in mainqt (comes from clicking)
	void updateNumPoints(int);
	/// Updates UI in mainqt (comes from clicking)
	void updateNumPointsToGenerate(int);

	/// This signal exists so that the mainqt knows to update enabled/disabled state of its button
	void imageLoaded();
	/// Indicates that the PDF computation was completed; ergo, updates UI in mainqt
	void setUsePDF(bool);
	/// Indicates that the algorithm has finished computing; ergo, updates UI in mainqt
	void setVoronoiComputed(bool);

public slots:
	/// For when the '...' button is clicked in mainqt
	void doOpenImage();

	/// 6x Slots, rather than just 'set render image kind' or so.
	void doDrawImage();
	void doDrawEdge();
	void doDrawEdgeSharp();
	void doDrawEdgeBlur();
	void doDrawPDF();
	void doDrawEffect();

	/// 2x Slots also related to draw-state.
	void setShowVoronoiSites(bool b);
	void setShowVoronoiEdges(bool b);

	/// Invoke various computations from Main Qt. ... TBH, might as well just do this implicitly.
	/// UX: can bold / unbold, for like ... 'available / not'?
	void doPDF();
	void doVoronoiDiagram();

	/// Control of point-generation.
	void setNumPoints1k();
	void setNumPoints5k();
	void setNumPoints(int);
	void doGenerateUniformRandomPoints();

	/// When the 'save image' button is clicked, from mainqt.
	void doSaveImage();

	/// A general reset. Compared to above, resets: filename, draw state, algorithm computations, input-points.
	void clearAll();


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
    delaunay::PointSetArray<delaunay::LongInt> inputPointSet_;
};

#endif // MYPANELOPENGL_H
