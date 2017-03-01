#-------------------------------------------------
#
# Project created by QtCreator 2017-03-01T20:51:47
#
#-------------------------------------------------

QT       -= core

TARGET = gui_library
TEMPLATE = lib
CONFIG += staticlib

SOURCES += gui_library.cpp

HEADERS += gui_library.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
