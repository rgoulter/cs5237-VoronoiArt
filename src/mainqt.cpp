#include "mainqt.h"

#include <iostream>
#include <utility>
#include <vector>

#include <QDebug>
#include <QFileDialog>
#include <QTextStream>
#include <QThreadPool>
#include <QtWidgets/QApplication>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "delaunay/delaunay.h"
#include "delaunay/pointsetarray.h"

#include "ui/qt5/delaunay.h"
#include "ui/qt5/voronoieffect.h"

#include "generatepoints.h"
#include "imagedata.h"

using std::pair;
using std::vector;

using cv::Mat;
using cv::imread;

using delaunay::LongInt;
using delaunay::PointSetArray;

using ui::qt5::Delaunay;
using ui::qt5::EffectState;
using ui::qt5::ShowImageType;
using ui::qt5::VoronoiEffect;



mainqt::mainqt(QWidget *parent)
	: QMainWindow(parent) {
	ui.setupUi(this);

	connect(ui.btnLoadImage, &QAbstractButton::pressed, this, &mainqt::chooseImage);

	connect(ui.radioBtnEffectNone, &QAbstractButton::pressed, [=] {
		ui.glWidget->getVoronoiEffect()->setEffectShowType(ShowImageType::NONE);
	});
	connect(ui.radioBtnEffectImage, &QAbstractButton::pressed, [=] {
		ui.glWidget->getVoronoiEffect()->setEffectShowType(ShowImageType::IMAGE);
	});
	connect(ui.radioBtnEffectEdges, &QAbstractButton::pressed, [=] {
		ui.glWidget->getVoronoiEffect()->setEffectShowType(ShowImageType::EDGE_RAW);
	});
	connect(ui.radioBtnEffectEdgesSharp, &QAbstractButton::pressed, [=] {
		ui.glWidget->getVoronoiEffect()->setEffectShowType(ShowImageType::EDGE_SHARP);
	});
	connect(ui.radioBtnEffectEdgesBlurred, &QAbstractButton::pressed, [=] {
		ui.glWidget->getVoronoiEffect()->setEffectShowType(ShowImageType::EDGE_BLUR);
	});
	connect(ui.radioBtnEffectPDF, &QAbstractButton::pressed, [=] {
		ui.glWidget->getVoronoiEffect()->setEffectShowType(ShowImageType::PDF);
	});

	connect(ui.radioBtnEffectVoronoi, &QAbstractButton::pressed, [=] {
		ui.chkShowAlgorithm->setEnabled(true);
		ui.chkShowEdges->setEnabled(true);

		const vector<pair<int, int>>& points = ui.glWidget->getPoints();
		const PointSetArray<LongInt>& inputPointSet(points);
		Delaunay* delaunay = new Delaunay(inputPointSet);
		ui.glWidget->getVoronoiEffect()->setDelaunayAlgorithm(delaunay);

		connect(delaunay, &Delaunay::progressed, [=] (int numProcessed, int total) {
			// Update the progress bar
			ui.progressBarVoronoi->setMaximum(total);
			ui.progressBarVoronoi->setValue(numProcessed);

			// Redraw the UI
			ui.glWidget->updateGL();

			// Once it's finished all iterations...
			if (numProcessed >= total) {
				ui.glWidget->getVoronoiEffect()->setVoronoiPolygons(delaunay->getVoronoiPolygons());
				ui.glWidget->getVoronoiEffect()->setEffectShowType(ShowImageType::EFFECT);
			}
		});

		QThreadPool::globalInstance()->start(delaunay);
	});

	connect(ui.btnGenUniform, &QAbstractButton::pressed, [=] {
		int numPoints = ui.spinBoxNumPoints->value();
		ImageData* imageData = ui.glWidget->getVoronoiEffect()->getImageData();
		int width = imageData->width();
		int height = imageData->height();
		vector<pair<int, int>> points = generateUniformRandomPoints(width, height, numPoints);
		ui.glWidget->insertPoints(points);
	});
	connect(ui.btnGenPDF, &QAbstractButton::pressed, [=] {
		int numPoints = ui.spinBoxNumPoints->value();
		VoronoiEffect* voronoiEffect = ui.glWidget->getVoronoiEffect();

		ImageData* imageData = voronoiEffect->getImageData();
		PDFTextures pdfTextures = pdfTexturesFromImage(imageData->getImageMat());
		voronoiEffect->setPDFTextures(pdfTextures);
		setUsePDF(true);

		const Mat& pdfMat = distributionFromPDFTextures(pdfTextures);
		vector<pair<int, int>> points = generatePointsFromDistributionField(pdfMat, numPoints);
		ui.glWidget->insertPoints(points);
	});

	connect(ui.glWidget, &MyPanelOpenGL::hasEnoughPointsForVoronoiEffect, [=] {
		ui.radioBtnEffectVoronoi->setEnabled(true);
	});

	connect(ui.chkShowPoints, &QAbstractButton::toggled, [=] {
		EffectState currentState = ui.glWidget->getVoronoiEffect()->getEffectState();
		currentState.showVertices = !currentState.showVertices;
		ui.glWidget->getVoronoiEffect()->setEffectState(currentState);
	});
	connect(ui.chkShowEdges, &QAbstractButton::toggled, [=] {
		EffectState currentState = ui.glWidget->getVoronoiEffect()->getEffectState();
		currentState.showEdges = !currentState.showEdges;
		ui.glWidget->getVoronoiEffect()->setEffectState(currentState);
	});
	connect(ui.chkShowAlgorithm, &QAbstractButton::toggled, [=] {
		EffectState currentState = ui.glWidget->getVoronoiEffect()->getEffectState();
		currentState.showAlgorithm = !currentState.showAlgorithm;
		ui.glWidget->getVoronoiEffect()->setEffectState(currentState);
	});

	connect(ui.btnSaveImage, &QAbstractButton::pressed, ui.glWidget, &MyPanelOpenGL::saveImage);

	connect(ui.btnClearAll, &QAbstractButton::pressed, this, &mainqt::clearAll);
}



mainqt::~mainqt() { }



void mainqt::chooseImage() {
	//get a filename to open
	QString qStr_fileName =
		QFileDialog::getOpenFileName(Q_NULLPTR,
	                                 tr("Open Image"),
	                                 ".",
	                                 tr("Image Files (*.png *.jpg *.bmp)"));
	if (qStr_fileName == "") {
		return;
	}

	clearAll();

	ui.txtImageName->setText(qStr_fileName);

	ImageData* imageData = loadImageData(qStr_fileName.toStdString());
	ui.glWidget->getVoronoiEffect()->setImageData(imageData);

	imageLoaded();

	ui.radioBtnEffectImage->setChecked(true);
}



void mainqt::imageLoaded() {
	ui.chkShowPoints->setEnabled(true);

	ui.radioBtnEffectImage->setEnabled(true);

	ui.spinBoxNumPoints->setEnabled(true);
	ui.btnGenUniform->setEnabled(true);
	ui.btnGenPDF->setEnabled(true);

	ui.btnSaveImage->setEnabled(true);
	ui.btnClearAll->setEnabled(true);
}

void mainqt::setUsePDF(bool b) {
	ui.radioBtnEffectEdges->setEnabled(b);
	ui.radioBtnEffectEdgesBlurred->setEnabled(b);
	ui.radioBtnEffectEdgesSharp->setEnabled(b);
	ui.radioBtnEffectPDF->setEnabled(b);
}



void mainqt::clearAll() {
	ui.txtImageName->setText("");

	ui.radioBtnEffectImage->setEnabled(false);
	ui.radioBtnEffectEdges->setEnabled(false);
	ui.radioBtnEffectEdgesBlurred->setEnabled(false);
	ui.radioBtnEffectEdgesSharp->setEnabled(false);
	ui.radioBtnEffectPDF->setEnabled(false);
	ui.radioBtnEffectVoronoi->setEnabled(false);

	ui.radioBtnEffectNone->setChecked(true);

	ui.spinBoxNumPoints->setEnabled(false);
	ui.btnGenUniform->setEnabled(false);
	ui.btnGenPDF->setEnabled(false);
	
	ui.chkShowPoints->setEnabled(false);
	ui.chkShowEdges->setEnabled(false);
	ui.chkShowAlgorithm->setEnabled(false);

	ui.progressBarVoronoi->setMaximum(100);
	ui.progressBarVoronoi->setValue(0);

	ui.btnSaveImage->setEnabled(false);
	ui.btnClearAll->setEnabled(false);

	ui.glWidget->clearAll();
}



int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	mainqt w;
	w.show();
	return a.exec();
}

