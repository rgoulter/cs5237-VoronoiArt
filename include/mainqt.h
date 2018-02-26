#ifndef MAINQT_H
#define MAINQT_H

#include <string>

#include <QtWidgets/QMainWindow>
#include "ui_mainqt.h"

#include "ui/qt5/delaunay.h"


class mainqt : public QMainWindow {
	Q_OBJECT

public:
	mainqt(QWidget *parent = 0);
	~mainqt();

public slots:
	/// Slot so that we can update the GLWidget from the UI thread.
	void algorithmProgressed(int numProcessed, int total);

	void chooseImage();

	/// A general reset. Compared to above, resets: filename, draw state, algorithm computations, input-points.
	void clearAll();

	// Just copy-paste the name of the signals from MyPanelOpenGL
	void imageLoaded();

	void setUsePDF(bool);

private:
	Ui::mainqtClass ui;
	ui::qt5::Delaunay* delaunay_ = nullptr;

};

#endif // MAINQT_H
