#-------------------------------------------------
#
# Project created by QtCreator 2017-03-01T20:51:47
#
#-------------------------------------------------

QT       -= core
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gui_library
TEMPLATE = lib
CONFIG += staticlib

SOURCES += gui_library.cpp \
    intro_window.cpp

HEADERS += gui_library.h \
    intro_window.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
