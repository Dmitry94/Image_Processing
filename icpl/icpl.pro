#-------------------------------------------------
#
# Project created by QtCreator 2017-03-12T17:15:56
#
#-------------------------------------------------

QT       -= core gui

TARGET = icpl
TEMPLATE = lib

CONFIG += staticlib c++14
CONFIG += omp optimization
CONFIG += opencv


DEFINES += ICPL_LIBRARY

SOURCES += \
    utils.cpp

HEADERS += icpl.h \
    utils.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}


opencv {
    LIBS += -lopencv_core -lopencv_imgproc
}

optimization {
    QMAKE_CFLAGS_RELEASE       += -O2
    QMAKE_CFLAGS_DEBUG         += -g
}

omp {
    LIBS += -fopenmp
    QMAKE_CXXFLAGS += -fopenmp
}
