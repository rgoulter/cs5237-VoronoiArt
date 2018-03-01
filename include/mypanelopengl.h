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

	void inputPointsChanged();

public slots:
	/// When the 'save image' button is clicked, from mainqt.
	void saveImage();

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

private:
	// TODO Probably could do without these?
	// Position of the image, in canvas space..
	int canvasOffsetX_ = 0;
	int canvasOffsetY_ = 0;

	ui::qt5::VoronoiEffect* effect_ = nullptr;

	// DELAUNAY
	std::vector<std::pair<int, int>> inputPoints_;
};

#endif // MYPANELOPENGL_H
