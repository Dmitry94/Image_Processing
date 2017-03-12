TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

unix: LIBS += -L$$OUT_PWD/../icpl/ -licpl

INCLUDEPATH += $$PWD/../icpl
DEPENDPATH += $$PWD/../icpl

unix: PRE_TARGETDEPS += $$OUT_PWD/../icpl/libicpl.a

LIBS += -lopencv_core -lopencv_imgproc

unix: LIBS += -L$$OUT_PWD/../l/ -ll

INCLUDEPATH += $$PWD/../l
DEPENDPATH += $$PWD/../l

unix: PRE_TARGETDEPS += $$OUT_PWD/../l/libl.a
