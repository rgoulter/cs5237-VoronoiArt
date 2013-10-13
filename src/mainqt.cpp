#include "mainqt.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include <QTextStream>
#include <QDebug>

using namespace std;

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
	QTextStream out(stdout);
	out << "Running QT now.." << endl << flush;
	qDebug()<<"debug output"<<endl;

	QApplication a(argc, argv);
	mainqt w;
	w.show();
	return a.exec();
}
