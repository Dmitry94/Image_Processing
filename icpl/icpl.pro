#-------------------------------------------------
#
# Project created by QtCreator 2017-03-12T17:15:56
#
#-------------------------------------------------

QT       -= core gui

TARGET = icpl
TEMPLATE = lib

CONFIG += staticlib
CONFIG += c++14
CONFIG += omp optimization


DEFINES += ICPL_LIBRARY

SOURCES += \
    utils.cpp

HEADERS += icpl.h \
    utils.h

optimization {
    QMAKE_CFLAGS_RELEASE       += -O2
    QMAKE_CFLAGS_DEBUG         += -g
}

omp {
    QMAKE_CXXFLAGS += -fopenmp
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}
