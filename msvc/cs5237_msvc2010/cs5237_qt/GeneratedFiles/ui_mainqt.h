/********************************************************************************
** Form generated from reading UI file 'mainqt.ui'
**
** Created by: Qt User Interface Compiler version 5.1.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINQT_H
#define UI_MAINQT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>
#include "mypanelopengl.h"

QT_BEGIN_NAMESPACE

class Ui_mainqtClass
{
public:
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    MyPanelOpenGL *glWidget;
    QWidget *widget_3;
    QPushButton *btnDoDelaunay;
    QLabel *labelLoadImage;
    QPushButton *btnLoadImage;
    QLineEdit *txtImageName;
    QPushButton *btnDrawImage;
    QPushButton *btnDrawEdges;
    QPushButton *btnDrawPDF;
    QFrame *line;
    QSpinBox *spinBoxNumPoints;
    QLabel *labelNumPoints;
    QPushButton *btnGenUniform;
    QPushButton *btnGenPDF;
    QFrame *line_2;
    QPushButton *btnDoVoronoi;
    QPushButton *btnClearAll;

    void setupUi(QMainWindow *mainqtClass)
    {
        if (mainqtClass->objectName().isEmpty())
            mainqtClass->setObjectName(QStringLiteral("mainqtClass"));
        mainqtClass->resize(621, 499);
        centralWidget = new QWidget(mainqtClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, -1, 0, -1);
        glWidget = new MyPanelOpenGL(centralWidget);
        glWidget->setObjectName(QStringLiteral("glWidget"));

        horizontalLayout->addWidget(glWidget);

        widget_3 = new QWidget(centralWidget);
        widget_3->setObjectName(QStringLiteral("widget_3"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget_3->sizePolicy().hasHeightForWidth());
        widget_3->setSizePolicy(sizePolicy);
        widget_3->setMinimumSize(QSize(200, 0));
        btnDoDelaunay = new QPushButton(widget_3);
        btnDoDelaunay->setObjectName(QStringLiteral("btnDoDelaunay"));
        btnDoDelaunay->setGeometry(QRect(10, 280, 181, 23));
        btnDoDelaunay->setFlat(false);
        labelLoadImage = new QLabel(widget_3);
        labelLoadImage->setObjectName(QStringLiteral("labelLoadImage"));
        labelLoadImage->setGeometry(QRect(0, 10, 71, 16));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(labelLoadImage->sizePolicy().hasHeightForWidth());
        labelLoadImage->setSizePolicy(sizePolicy1);
        btnLoadImage = new QPushButton(widget_3);
        btnLoadImage->setObjectName(QStringLiteral("btnLoadImage"));
        btnLoadImage->setGeometry(QRect(160, 30, 31, 23));
        txtImageName = new QLineEdit(widget_3);
        txtImageName->setObjectName(QStringLiteral("txtImageName"));
        txtImageName->setGeometry(QRect(2, 30, 151, 20));
        txtImageName->setReadOnly(false);
        btnDrawImage = new QPushButton(widget_3);
        btnDrawImage->setObjectName(QStringLiteral("btnDrawImage"));
        btnDrawImage->setGeometry(QRect(0, 60, 191, 23));
        btnDrawEdges = new QPushButton(widget_3);
        btnDrawEdges->setObjectName(QStringLiteral("btnDrawEdges"));
        btnDrawEdges->setEnabled(false);
        btnDrawEdges->setGeometry(QRect(0, 90, 191, 23));
        btnDrawPDF = new QPushButton(widget_3);
        btnDrawPDF->setObjectName(QStringLiteral("btnDrawPDF"));
        btnDrawPDF->setEnabled(false);
        btnDrawPDF->setGeometry(QRect(0, 120, 191, 23));
        line = new QFrame(widget_3);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(0, 150, 191, 16));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        spinBoxNumPoints = new QSpinBox(widget_3);
        spinBoxNumPoints->setObjectName(QStringLiteral("spinBoxNumPoints"));
        spinBoxNumPoints->setGeometry(QRect(131, 170, 61, 22));
        spinBoxNumPoints->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        labelNumPoints = new QLabel(widget_3);
        labelNumPoints->setObjectName(QStringLiteral("labelNumPoints"));
        labelNumPoints->setGeometry(QRect(10, 170, 111, 21));
        btnGenUniform = new QPushButton(widget_3);
        btnGenUniform->setObjectName(QStringLiteral("btnGenUniform"));
        btnGenUniform->setGeometry(QRect(10, 200, 181, 23));
        btnGenPDF = new QPushButton(widget_3);
        btnGenPDF->setObjectName(QStringLiteral("btnGenPDF"));
        btnGenPDF->setGeometry(QRect(10, 230, 181, 23));
        line_2 = new QFrame(widget_3);
        line_2->setObjectName(QStringLiteral("line_2"));
        line_2->setGeometry(QRect(0, 260, 191, 16));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);
        btnDoVoronoi = new QPushButton(widget_3);
        btnDoVoronoi->setObjectName(QStringLiteral("btnDoVoronoi"));
        btnDoVoronoi->setEnabled(true);
        btnDoVoronoi->setGeometry(QRect(10, 310, 181, 23));
        btnClearAll = new QPushButton(widget_3);
        btnClearAll->setObjectName(QStringLiteral("btnClearAll"));
        btnClearAll->setGeometry(QRect(14, 450, 181, 23));
        line->raise();
        btnDoDelaunay->raise();
        labelLoadImage->raise();
        btnLoadImage->raise();
        txtImageName->raise();
        btnDrawImage->raise();
        btnDrawEdges->raise();
        btnDrawPDF->raise();
        spinBoxNumPoints->raise();
        labelNumPoints->raise();
        btnGenUniform->raise();
        btnGenPDF->raise();
        line_2->raise();
        btnDoVoronoi->raise();
        btnClearAll->raise();

        horizontalLayout->addWidget(widget_3);

        mainqtClass->setCentralWidget(centralWidget);

        retranslateUi(mainqtClass);
        QObject::connect(btnDoDelaunay, SIGNAL(clicked()), glWidget, SLOT(doDelaunayTriangulation()));
        QObject::connect(btnLoadImage, SIGNAL(clicked()), glWidget, SLOT(doOpenImage()));
        QObject::connect(glWidget, SIGNAL(updateFilename(QString)), txtImageName, SLOT(setText(QString)));
        QObject::connect(btnDrawImage, SIGNAL(clicked()), glWidget, SLOT(doDrawImage()));
        QObject::connect(btnDoVoronoi, SIGNAL(clicked()), glWidget, SLOT(doVoronoiDiagram()));
        QObject::connect(btnGenPDF, SIGNAL(clicked()), glWidget, SLOT(doPDF()));

        QMetaObject::connectSlotsByName(mainqtClass);
    } // setupUi

    void retranslateUi(QMainWindow *mainqtClass)
    {
        mainqtClass->setWindowTitle(QApplication::translate("mainqtClass", "mainqt", 0));
        btnDoDelaunay->setText(QApplication::translate("mainqtClass", "Delaunay Triangulate", 0));
        labelLoadImage->setText(QApplication::translate("mainqtClass", "Load Image:", 0));
        btnLoadImage->setText(QApplication::translate("mainqtClass", "...", 0));
        btnDrawImage->setText(QApplication::translate("mainqtClass", "Draw Image", 0));
        btnDrawEdges->setText(QApplication::translate("mainqtClass", "Draw Edges", 0));
        btnDrawPDF->setText(QApplication::translate("mainqtClass", "Draw PDF", 0));
        labelNumPoints->setText(QApplication::translate("mainqtClass", "No. points to generate:", 0));
        btnGenUniform->setText(QApplication::translate("mainqtClass", "Generate Uniform Random", 0));
        btnGenPDF->setText(QApplication::translate("mainqtClass", "Generate with PDF", 0));
        btnDoVoronoi->setText(QApplication::translate("mainqtClass", "Voronoi Diagram", 0));
        btnClearAll->setText(QApplication::translate("mainqtClass", "Clear All", 0));
    } // retranslateUi

};

namespace Ui {
    class mainqtClass: public Ui_mainqtClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINQT_H
