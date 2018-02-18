#include "mypanelopengl.h"

#include <cmath>
#include <cstdio>
#include <stdlib.h>

#include <QtGui/QMouseEvent>
#include <QDebug>
#include <QString>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <string>
#include <utility>

#include "platform.h"

#include "delaunay/longint/li.h"

#include "delaunay/pointsetarray.h"
#include "delaunay/delaunay.h"

#include "ui/opengl/graphics.h"

#include "imagedata.h"

using cv::Mat;
using cv::imread;

using std::make_pair;
using std::pair;
using std::string;
using std::vector;

using delaunay::LongInt;
using delaunay::PointSetArray;

using geometry::Polygon;

using ui::qt5::VoronoiEffect;
using ui::qt5::EffectState;



MyPanelOpenGL::MyPanelOpenGL(QWidget *parent) : QGLWidget (parent) {
	effect_ = new VoronoiEffect();

	connect(effect_, &VoronoiEffect::imageLoaded, [=] {
		QSize size = this->size();
		refreshProjection(size.width(), size.height(), canvasOffsetX_, canvasOffsetY_, effect_->getImageData());

		updateGL();
	});
	connect(effect_, &VoronoiEffect::effectChanged, [=] {
		updateGL();
	});
}



VoronoiEffect* MyPanelOpenGL::getVoronoiEffect() {
	return effect_;
}



void MyPanelOpenGL::initializeGL() {
	glShadeModel(GL_SMOOTH);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClearDepth(1.0f);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}



void MyPanelOpenGL::resizeGL(int width, int height) {
	refreshProjection(width, height, canvasOffsetX_, canvasOffsetY_, effect_->getImageData());
}



void MyPanelOpenGL::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	effect_->paintGL();

	if (effect_->getEffectState().showVertices) {
		for (const pair<int, int>& pt : inputPoints_) {
			drawAPoint(pt.first, pt.second);
		}
	}

	glPopMatrix();
}



void MyPanelOpenGL::mousePressEvent(QMouseEvent *event) {
	qDebug("[MyPanelOpenGL::mousePressEvent] x: %d, y: %d\n", event->x(), event->y());

	ImageData* imageData = effect_->getImageData();
	if (imageData == nullptr) {
		return;
	}

	int loadedImageWidth = imageData->width();
	int loadedImageHeight = imageData->height();

	QSize widgetSize = size();
	int windowWidth = widgetSize.width();
	int windowHeight = widgetSize.height();

	double imageRatio = ((double) loadedImageWidth) / loadedImageHeight;
	double windowRatio = ((double) windowWidth) / windowHeight;

	int renderWidth = 2;
	int renderHeight = 2;

	int deltaX = 1;
	int deltaY = 1;

	if (loadedImageWidth > 0) {
		if (imageRatio > windowRatio) {
			double ratio = ((double) windowWidth) / windowHeight;

			renderWidth = loadedImageWidth;
			renderHeight = (int) (loadedImageWidth / ratio);

			deltaY = (renderHeight - loadedImageHeight) / 2;
		} else {
			double ratio = ((double) windowWidth) / windowHeight;

			renderWidth = (int) (loadedImageHeight * ratio);
			renderHeight = loadedImageHeight;

			deltaX = (renderWidth - loadedImageWidth) / 2;
		}

		double viewScale = (double) renderWidth / windowWidth;

		int px = (event->x() * viewScale) - deltaX;
		int py = (event->y() * viewScale) - deltaY;

		qDebug("[MyPanelOpenGL::mousePressEvent] add point: px: %d, py: %d\n", px, py);
		insertPoint(px, py);
	}
}



/// coordinates x, y are in the same scale as the rendered image.
void MyPanelOpenGL::insertPoint(int x, int y) {
	inputPoints_.push_back(make_pair(x, y));

	const int minPointsForDelaunay = 3;
	if (inputPoints_.size() > minPointsForDelaunay) {
		emit hasEnoughPointsForVoronoiEffect();
	}

	updateGL();
}



void MyPanelOpenGL::insertPoints(std::vector<std::pair<int, int>> points) {
	for (const pair<int, int>& pt : points) {
		insertPoint(pt.first, pt.second);
	}
}



// void MyPanelOpenGL::doOpenImage() {
// 	//get a filename to open
// 	QString qStr_fileName =
// 		QFileDialog::getOpenFileName(Q_NULLPTR,
// 	                                 tr("Open Image"),
// 	                                 ".",
// 	                                 tr("Image Files (*.png *.jpg *.bmp)"));
// 	if (qStr_fileName == "") {
// 		return;
// 	}

// 	string filenameStr = qStr_fileName.toStdString();

// 	emit updateFilename(qStr_fileName); // to Qt textbox

// 	imData_ = loadImageData(filenameStr);
// 	loadedImageFilename_ = filenameStr;

// 	QSize widgetSize = size();
// 	refreshProjection(widgetSize.width(), widgetSize.height(),
// 	                  canvasOffsetX_, canvasOffsetY_,
// 	                  imData_);
// 	emit imageLoaded();

// 	currentRenderType_ = IMAGE;
// 	updateGL();
// }



void MyPanelOpenGL::saveImage() {
	// TODO: This impl. feels impure
	// TODO: Should be a way to set output filename
	// TODO: ImageData stuff applies here, too.
	string outputImageFilename = "output.bmp";

	int x = canvasOffsetX_;
	int y = canvasOffsetY_;

	QSize widgetSize = size();
	int windowWidth = widgetSize.width();
	int windowHeight = widgetSize.height();

	int copyWidth = (windowWidth - (2 * x));
	int copyHeight = (windowHeight - (2 * y));

	int numComponents = 3; // RGB

	// Memory for width * height * RGB pixel values.
	unsigned char *data;
	data = (unsigned char *) malloc(numComponents * copyWidth * copyHeight * sizeof(unsigned char));

	glPixelStorei(GL_PACK_ALIGNMENT, 1); // align to the byte..
	glReadBuffer(GL_FRONT);

	// Read in "correct" row...
	// (I think I have to do this since the texture coordinates' y-axis is flipped
	//  compared to the rendered y-axis).

	for (int rowOffset = 0; rowOffset < copyHeight; rowOffset++) {
		int copyRow = y + copyHeight - rowOffset - 1; // reverse this.
		unsigned char * copyAddress = data + rowOffset * (copyWidth * numComponents);
		glReadPixels(x, copyRow, copyWidth, 1, GL_BGR, GL_UNSIGNED_BYTE, copyAddress);
	}

	Mat img(copyHeight, copyWidth, CV_8UC3, data);
	imwrite(outputImageFilename.c_str(), img);
}





void MyPanelOpenGL::clearAll() {
	inputPoints_.clear();

	effect_->clearAll();

	updateGL();
}
