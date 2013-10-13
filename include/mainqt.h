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

private:
	Ui::mainqtClass ui;
};

#endif // MAINQT_H
