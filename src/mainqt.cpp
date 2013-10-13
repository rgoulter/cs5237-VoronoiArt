#include "mainqt.h"
#include <QtWidgets/QApplication>

mainqt::mainqt(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

mainqt::~mainqt()
{

}


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	mainqt w;
	w.show();
	return a.exec();
}
