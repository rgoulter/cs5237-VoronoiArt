#ifndef MAINQT_H
#define MAINQT_H

#include <string>

#include <QtWidgets/QMainWindow>
#include "ui_mainqt.h"



class mainqt : public QMainWindow {
	Q_OBJECT

public:
	mainqt(QWidget *parent = 0);
	~mainqt();

public slots:
	// Just copy-paste the name of the signals from MyPanelOpenGL
	void updateNumPoints(int);
	void imageLoaded();
	void setUsePDF(bool);
	void setVoronoiComputed(bool);

	void chooseImage();

	// XXX from mypanelOpenGL
	/// For when the '...' button is clicked in mainqt
	// void doOpenImage();
	/// 6x Slots, rather than just 'set render image kind' or so.
	// void doDrawImage();
	// void doDrawEdge();
	// void doDrawEdgeSharp();
	// void doDrawEdgeBlur();
	// void doDrawPDF();
	// void doDrawEffect();
	
	/// Invoke various computations from Main Qt. ... TBH, might as well just do this implicitly.
	/// UX: can bold / unbold, for like ... 'available / not'?
	// void doPDF();
	// void doVoronoiDiagram();

	/// Control of point-generation.
	// void setNumPoints1k();
	// void setNumPoints5k();
	// void setNumPoints(int);
	// void doGenerateUniformRandomPoints();

	/// When the 'save image' button is clicked, from mainqt.
	// void doSaveImage();

	/// A general reset. Compared to above, resets: filename, draw state, algorithm computations, input-points.
	void clearAll();

private:
	Ui::mainqtClass ui;

};

#endif // MAINQT_H
