#include "mainqt.h"

#include <iostream>
#include <utility>
#include <vector>

#include <QTextStream>
#include <QDebug>
#include <QFileDialog>
#include <QtWidgets/QApplication>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "ui/qt5/voronoieffect.h"

#include "generatepoints.h"
#include "imagedata.h"

using std::pair;
using std::vector;

using cv::Mat;
using cv::imread;

using ui::qt5::ShowImageType;
using ui::qt5::VoronoiEffect;



mainqt::mainqt(QWidget *parent)
	: QMainWindow(parent) {
	ui.setupUi(this);

	connect(ui.btnLoadImage, &QAbstractButton::pressed, this, &mainqt::chooseImage);

	connect(ui.radioBtnEffectNone, &QAbstractButton::pressed, [=]{
		ui.glWidget->getVoronoiEffect()->setEffectShowType(ShowImageType::NONE);
	});
	connect(ui.radioBtnEffectImage, &QAbstractButton::pressed, [=]{
		ui.glWidget->getVoronoiEffect()->setEffectShowType(ShowImageType::IMAGE);
	});
	connect(ui.radioBtnEffectEdges, &QAbstractButton::pressed, [=]{
		ui.glWidget->getVoronoiEffect()->setEffectShowType(ShowImageType::EDGE_RAW);
	});
	connect(ui.radioBtnEffectEdgesSharp, &QAbstractButton::pressed, [=]{
		ui.glWidget->getVoronoiEffect()->setEffectShowType(ShowImageType::EDGE_SHARP);
	});
	connect(ui.radioBtnEffectEdgesBlurred, &QAbstractButton::pressed, [=]{
		ui.glWidget->getVoronoiEffect()->setEffectShowType(ShowImageType::EDGE_BLUR);
	});
	connect(ui.radioBtnEffectPDF, &QAbstractButton::pressed, [=]{
		ui.glWidget->getVoronoiEffect()->setEffectShowType(ShowImageType::PDF);
	});

	connect(ui.btnGenUniform, &QAbstractButton::pressed, [=]{
		int numPoints = ui.spinBoxNumPoints->value();
		ImageData* imageData = ui.glWidget->getVoronoiEffect()->getImageData();
		int width = imageData->width();
		int height = imageData->height();
		vector<pair<int, int>> points = generateUniformRandomPoints(width, height, numPoints);
		ui.glWidget->insertPoints(points);
	});
	connect(ui.btnGenPDF, &QAbstractButton::pressed, [=]{
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
}



mainqt::~mainqt() { }



// XXX: method to "do load image", copied from mypanelopenglh (ish)
//      & set the image to the ... effect of the mypanelopengh

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

	ui.txtImageName->setText(qStr_fileName);

	ImageData* imageData = loadImageData(qStr_fileName.toStdString());
	ui.glWidget->getVoronoiEffect()->setImageData(imageData);

	imageLoaded();

	ui.radioBtnEffectImage->setChecked(true);
}



void mainqt::updateNumPoints(int n) {
	// bool b = n > 0;

	// ui.btnDoVoronoi->setEnabled(b);
}



void mainqt::imageLoaded() {
	// Enable various components
	ui.chkShowPoints->setEnabled(true);

	ui.radioBtnEffectImage->setEnabled(true);

	// ui.radioBtnEffectEdges->setEnabled(true);
	// ui.radioBtnEffectEdgesBlurred->setEnabled(true);
	// ui.radioBtnEffectEdgesSharp->setEnabled(true);
	// ui.radioBtnEffectPDF->setEnabled(true);

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



void mainqt::setVoronoiComputed(bool b) {
	// ui.btnDrawEffect->setEnabled(b);
	// ui.chkShowEdges->setEnabled(b);
}



int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	mainqt w;
	w.show();
	return a.exec();
}

