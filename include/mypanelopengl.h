#ifndef MYPANELOPENGL_H
#define MYPANELOPENGL_H

#include <string>
#include <utility>
#include <vector>

#include <QGLWidget>
#include <QString>

#include "delaunay/longint/li.h"

#include "delaunay/pointsetarray.h"

#include "geometry/polygon.h"

#include "ui/qt5/voronoieffect.h"

#include "polypixel.h"
#include "generatepoints.h"



// This is the QT5 Widget we use for:
//  - allowing click-input of points
//  - rendering the stuff using OpenGL
// and at the moment it happens to also:
//  - load image (to an ImageData), .... because .... ???
//  - save image (because of the OpenGL rendering... fair?)
//  - keep the algorithm's state ... and render it...
//
// As per http://doc.qt.io/qt-5/qglwidget.html
// XXX QGLWidget is legacy, should use QOpenGLWidget instead.
class MyPanelOpenGL : public QGLWidget
{
	Q_OBJECT
public:
	explicit MyPanelOpenGL(QWidget *parent = 0);

	ui::qt5::VoronoiEffect* getVoronoiEffect();

	void insertPoint(int x, int y);
	void insertPoints(std::vector<std::pair<int, int>> points);

	const std::vector<std::pair<int, int>>& getPoints() const { return inputPoints_; };

signals:
	void hasEnoughPointsForVoronoiEffect();
	// /// Provides a way to update the *textfield* in mainqt
	// void updateFilename(QString);

	// /// This signal exists so that the mainqt knows to update enabled/disabled state of its button
	// void imageLoaded();
	// /// Indicates that the PDF computation was completed; ergo, updates UI in mainqt
	// void setUsePDF(bool);
	// /// Indicates that the algorithm has finished computing; ergo, updates UI in mainqt
	// void setVoronoiComputed(bool);

public slots:
	// /// For when the '...' button is clicked in mainqt
	// void doOpenImage();

	// /// 6x Slots, rather than just 'set render image kind' or so.
	// void doDrawImage();
	// void doDrawEdge();
	// void doDrawEdgeSharp();
	// void doDrawEdgeBlur();
	// void doDrawPDF();
	// void doDrawEffect();

	// /// 2x Slots also related to draw-state.
	// void setShowVoronoiSites(bool b);
	// void setShowVoronoiEdges(bool b);

	// /// Invoke various computations from Main Qt. ... TBH, might as well just do this implicitly.
	// /// UX: can bold / unbold, for like ... 'available / not'?
	// void doPDF();
	// void doVoronoiDiagram();

	// /// Control of point-generation.
	// void setNumPoints1k();
	// void setNumPoints5k();
	// void setNumPoints(int);
	// void doGenerateUniformRandomPoints();

	// /// When the 'save image' button is clicked, from mainqt.
	// void doSaveImage();

	// /// A general reset. Compared to above, resets: filename, draw state, algorithm computations, input-points.
	void clearAll();


protected:
	// overloaded
	void initializeGL();
	// overloaded
	void resizeGL(int x, int h);
	// overloaded
	void paintGL();
	void mousePressEvent(QMouseEvent *event);

private:
	// bool hasLoadedImage() { return false; }

	// TODO Probably could do without these?
	// Position of the image, in canvas space..
	int canvasOffsetX_ = 0;
	int canvasOffsetY_ = 0;

	ui::qt5::VoronoiEffect* effect_ = nullptr;


	// DELAUNAY
	std::vector<std::pair<int, int>> inputPoints_;
};

#endif // MYPANELOPENGL_H
