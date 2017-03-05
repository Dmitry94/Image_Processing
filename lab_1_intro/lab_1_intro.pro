#-------------------------------------------------
#
# Project created by QtCreator 2017-03-01T23:07:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = lab_1_intro
TEMPLATE = app

CONFIG += gui_library

SOURCES += main.cpp

gui_library {
    unix: LIBS += -L$$OUT_PWD/../gui_library/ -lgui_library
    INCLUDEPATH += $$PWD/../gui_library
    DEPENDPATH += $$PWD/../gui_library
    unix: PRE_TARGETDEPS += $$OUT_PWD/../gui_library/libgui_library.a
}
