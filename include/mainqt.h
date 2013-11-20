#ifndef MAINQT_H
#define MAINQT_H

#include <QtWidgets/QMainWindow>
#include "ui_mainqt.h"

class mainqt : public QMainWindow
{
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

private:
	Ui::mainqtClass ui;
};

#endif // MAINQT_H