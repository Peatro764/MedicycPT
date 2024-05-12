QMAKE_CXXFLAGS += -std=c++11
QT +=  core \
    gui \
    sql \
    network \
    testlib \
    widgets
CONFIG += qt warn_on depend_includepath testcase
    TEMPLATE = app
    TARGET = TestUtil
    HEADERS += TestUtil.h
    SOURCES += TestUtil.cpp testmain.cpp

LIBS += -L$$OUT_PWD/../src/ -lClientConnection

INCLUDEPATH += $$PWD/../src
DEPENDPATH += $$PWD/../src
