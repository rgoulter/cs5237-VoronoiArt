#-------------------------------------------------
# CS5237 Voronoi Art
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += opengl


TARGET = cs5237qt_linux
TEMPLATE = app


LIBS += -lGL
LIBS += -lGLU
LIBS += -lGLEW

# TODO: If Linux..
LIBS += -lSOIL

LIBS += -lopencv_core
LIBS += -lopencv_highgui
LIBS += -lopencv_imgproc



SOURCES += \
    $$files(../../src/*.cpp) \
    $$files(../../src/basics/*.cpp) \
    $$files(../../src/basicsP2/*.cpp) \
    $$files(../../src/basicsP3/*.cpp) \
    $$files(../../src/linux/b) \
    ../../src/linux/basics/stopWatch.cpp

# This has main, so we will exclude it from Qt build
# TODO: Use QMake conditional to check whether to exclude this, or Qt main..
SOURCES -= ../../src/polyAvgShaderTestDriver.cpp


# Why does QMake need me to add the header files?
# (I know I could use wildcars, but still..)
HEADERS  += \
    $$files(../../include/*.h) \
    $$files(../../include/basics/*.h) \
    $$files(../../include/basicsP2/*.h) \
    $$files(../../include/basicsP3/*.h) \
    ../../include/linux/platform.h \
    ../../include/linux/basics/stopWatch.h



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
