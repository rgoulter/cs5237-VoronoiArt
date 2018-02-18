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
	void chooseImage();

	/// A general reset. Compared to above, resets: filename, draw state, algorithm computations, input-points.
	void clearAll();

	// Just copy-paste the name of the signals from MyPanelOpenGL
	void imageLoaded();

	void setUsePDF(bool);

private:
	Ui::mainqtClass ui;

};

#endif // MAINQT_H
