#-------------------------------------------------
#
# Project created by QtCreator 2013-10-14T22:10:03
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += opengl

TARGET = cs5237qt_linux
TEMPLATE = app

QMAKE_CXXFLAGS += -lGL
QMAKE_CXXFLAGS += -lGLU

SOURCES += \
    ../../src/mypanelopengl.cpp \
    ../../src/mainqt.cpp \
    ../../src/linux/basics/stopWatch.cpp \
    ../../src/basics/pointSet.cpp \
    ../../src/basics/lmath.cpp \
    ../../src/basics/li.cpp \
    ../../src/basicsP2/trist.cpp \
    ../../src/basicsP2/pointSetArray.cpp \
    ../../src/basicsP3/directedGraph.cpp

HEADERS  += \
    ../../include/mypanelopengl.h \
    ../../include/mainqt.h \
    ../../include/delaunayTri.h \
    ../../include/basics/pointSet.h \
    ../../include/basics/lmath.h \
    ../../include/basics/li.h \
    ../../include/basicsP2/trist.h \
    ../../include/basicsP2/pointSetArray.h \
    ../../include/basicsP3/directedGraph.h \
    ../../include/linux/basics/stopWatch.h \
    ../../include/linux/platform.h

INCLUDEPATH += \
    ../../include/ \
    ../../include/basics/ \
    ../../include/basicsP2/ \
    ../../include/basicsP3/ \
    ../../include/linux/basics/ \
    ../../include/linux/

FORMS    += \
    ../../forms/mainqt.ui

RESOURCES += \
    ../../resource/mainqt.qrc
