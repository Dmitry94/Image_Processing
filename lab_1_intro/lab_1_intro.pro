#-------------------------------------------------
#
# Project created by QtCreator 2017-03-01T23:07:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = lab_1_Base
TEMPLATE = app

CONFIG += c++14
CONFIG += icpl
CONFIG += gui_library
CONFIG += console
CONFIG += opencv

HEADERS += \
    main_window.h

SOURCES += main.cpp \
    main_window.cpp

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
    LIBS += $$OUT_PWD/../icpl/libicpl.a
    LIBS += -fopenmp
}

opencv {
    LIBS += -lopencv_core -lopencv_imgproc
}
