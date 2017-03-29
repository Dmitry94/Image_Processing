#-------------------------------------------------
#
# Project created by QtCreator 2017-03-28T17:38:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = lab_2_bright
TEMPLATE = app

CONFIG += c++14
CONFIG += omp optimization
CONFIG += icpl
CONFIG += gui_library
CONFIG += opencv

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

gui_library {
    unix: LIBS += -L$$OUT_PWD/../gui_library/ -lgui_library
    INCLUDEPATH += $$PWD/../gui_library
    DEPENDPATH += $$PWD/../gui_library
    unix: PRE_TARGETDEPS += $$OUT_PWD/../gui_library/libgui_library.a
}

icpl {
    unix: LIBS += -L$$OUT_PWD/../icpl/ -licpl
    INCLUDEPATH += $$PWD/../icpl
    DEPENDPATH += $$PWD/../icpl
    unix: PRE_TARGETDEPS += $$OUT_PWD/../icpl/libicpl.a
    LIBS += -fopenmp
}

opencv {
    LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc
}

optimization {
    QMAKE_CFLAGS_RELEASE       += -O2
    QMAKE_CFLAGS_DEBUG         += -g
}

omp {
    QMAKE_CXXFLAGS += -fopenmp
}
