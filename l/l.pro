#-------------------------------------------------
#
# Project created by QtCreator 2017-03-12T22:38:00
#
#-------------------------------------------------

QT       -= core gui

TARGET = l
TEMPLATE = lib
CONFIG += staticlib

SOURCES += l.cpp

HEADERS += l.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
