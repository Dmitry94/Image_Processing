#-------------------------------------------------
#
# Project created by QtCreator 2017-03-01T20:51:47
#
#-------------------------------------------------

QT       -= core
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gui_library
TEMPLATE = lib
CONFIG += staticlib c++14

SOURCES += gui_library.cpp \
    base_window.cpp

HEADERS += gui_library.h \
    base_window.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
