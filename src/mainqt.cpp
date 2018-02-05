#include "mainqt.h"

#include <iostream>

#include <QTextStream>
#include <QDebug>
#include <QtWidgets/QApplication>



mainqt::mainqt(QWidget *parent)
	: QMainWindow(parent) {
	ui.setupUi(this);
}



mainqt::~mainqt() { }



void mainqt::updateNumPoints(int n) {
	bool b = n > 0;

	ui.btnDoVoronoi->setEnabled(b);
}



void mainqt::imageLoaded() {
	// Enable various components
	ui.btnDrawImage->setEnabled(true);
	ui.chkShowPoints->setEnabled(true);

	ui.spinBoxNumPoints->setEnabled(true);
	ui.btn1kPoints->setEnabled(true);
	ui.btn5kPoints->setEnabled(true);
	ui.btnGenUniform->setEnabled(true);
	ui.btnGenPDF->setEnabled(true);

	ui.btnSaveImage->setEnabled(true);
	ui.btnClearAll->setEnabled(true);
}

void mainqt::setUsePDF(bool b) {
	ui.btnDrawEdges->setEnabled(b);
	ui.btnDrawBlurredEdges->setEnabled(b);
	ui.btnDrawSharpEdges->setEnabled(b);
	ui.btnDrawPDF->setEnabled(b);
}



void mainqt::setVoronoiComputed(bool b) {
	ui.btnDrawEffect->setEnabled(b);
	ui.chkShowEdges->setEnabled(b);
}



int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	mainqt w;
	w.show();
	return a.exec();
}

