#ifndef MYPANELOPENGL_H
#define MYPANELOPENGL_H

#include <QGLWidget>

class MyPanelOpenGL : public QGLWidget
{
    Q_OBJECT
public:
    explicit MyPanelOpenGL(QWidget *parent = 0);
	
public slots:
	void doDelaunayTriangulation();
	void doVoronoiDiagram();

protected:
    void initializeGL();
    void resizeGL(int x, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:

signals:

public slots:

};

#endif // MYPANELOPENGL_H
