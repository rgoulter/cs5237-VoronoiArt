#ifndef MYPANELOPENGL_H
#define MYPANELOPENGL_H

#include <string>

#include <QGLWidget>
#include <QString>



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
	void doDelaunayTriangulation();
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

};

#endif // MYPANELOPENGL_H
