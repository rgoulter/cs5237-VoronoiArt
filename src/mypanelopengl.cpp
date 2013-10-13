#include <QtGui/QMouseEvent>
#include "mypanelopengl.h"
#include <cmath>
#include <cstdio>


MyPanelOpenGL::MyPanelOpenGL(QWidget *parent) :
    QGLWidget (parent)
{
}

void MyPanelOpenGL::initializeGL(){
    glShadeModel(GL_SMOOTH);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClearDepth(1.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void MyPanelOpenGL::resizeGL(int width, int height){
    double xMin = 0, xMax = 10, yMin = 0, yMax = 10;
    glViewport(0, 0, (GLint) width, (GLint) height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);

    if(width > height) {
        height = height ? height : 1;
        double newWidth = (xMax - xMin) * width / height;
        double difWidth = newWidth - (xMax - xMin);
        xMin = 0.0 + difWidth / 2.0;
        xMax = 10 + difWidth / 2.0;
    } else {
        width = width ? width : 1;
        double newHeight = (yMax - yMin) * width / height;
        double difHeight = newHeight - (yMax - yMin);
        yMin = 0.0 + difHeight/ 2;
        yMax = 10 + difHeight / 2;
    }

    glOrtho(xMin, xMax, yMin, yMax, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void MyPanelOpenGL::paintGL(){
    int sides = 3;
    double radius = 1.0;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslated(5.0, 5.0, 0.0);
    glLineWidth(1);
    glColor3f(0, 0.7f, 0.7f);

    glBegin(GL_POLYGON);
    for(int i = 0; i < sides; i++) {
        glVertex2f(radius * cos(i*2*3.1415 / sides),
                   radius * sin(i*2*3.1415 / sides));
    }
    glEnd();

    glLineWidth(2);
    glColor3f(0, 1, 0);
    glBegin(GL_LINE_LOOP);
    for(int i = 0; i < sides; i++) {
        glVertex2f(radius * cos(i*2*3.1415 / sides),
                   radius * sin(i*2*3.1415 / sides));
    }
    glEnd();
}


void MyPanelOpenGL::mousePressEvent(QMouseEvent *event) {

}

void MyPanelOpenGL::mouseMoveEvent(QMouseEvent *event) {
    printf("%d, %d\n", event->x(), event->y());
}

void MyPanelOpenGL::keyPressEvent(QKeyEvent* event) {
    switch(event->key()) {
    case Qt::Key_Escape:
        close();
        break;
    default:
        event->ignore();
        break;
    }
}