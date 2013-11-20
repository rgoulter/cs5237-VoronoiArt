#ifndef MYPANELOPENGL_H
#define MYPANELOPENGL_H

#include <string>
#include <QGLWidget>
#include <QString>

class MyPanelOpenGL : public QGLWidget
{
    Q_OBJECT
public:
    explicit MyPanelOpenGL(QWidget *parent = 0);
	
signals:
	void updateFilename(QString);

public slots:
	void doDelaunayTriangulation();
	void doOpenImage();
	void doDrawImage();
	void doVoronoiDiagram();
	void doPDF();
	void clearAll();

protected:
    void initializeGL();
    void resizeGL(int x, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:

};

#endif // MYPANELOPENGL_H
