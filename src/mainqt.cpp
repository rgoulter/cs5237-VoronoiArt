#include "mainqt.h"

#include <iostream>

#include <QTextStream>
#include <QDebug>
#include <QFileDialog>
#include <QtWidgets/QApplication>

#include "imagedata.h"



mainqt::mainqt(QWidget *parent)
	: QMainWindow(parent) {
	ui.setupUi(this);

	// XXX add radio button group

	connect(ui.btnLoadImage, &QAbstractButton::pressed, this, &mainqt::chooseImage);
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
}



void mainqt::updateNumPoints(int n) {
	// bool b = n > 0;

	// ui.btnDoVoronoi->setEnabled(b);
}



void mainqt::imageLoaded() {
	// Enable various components
	ui.chkShowPoints->setEnabled(true);

	ui.radioBtnEffectImage->setEnabled(true);
	ui.radioBtnEffectEdges->setEnabled(true);
	ui.radioBtnEffectEdgesBlurred->setEnabled(true);
	ui.radioBtnEffectEdgesSharp->setEnabled(true);
	ui.radioBtnEffectPDF->setEnabled(true);

	ui.spinBoxNumPoints->setEnabled(true);
	ui.btnGenUniform->setEnabled(true);
	ui.btnGenPDF->setEnabled(true);

	ui.btnSaveImage->setEnabled(true);
	ui.btnClearAll->setEnabled(true);
}

void mainqt::setUsePDF(bool b) {
	// ui.btnDrawEdges->setEnabled(b);
	// ui.btnDrawBlurredEdges->setEnabled(b);
	// ui.btnDrawSharpEdges->setEnabled(b);
	// ui.btnDrawPDF->setEnabled(b);
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

