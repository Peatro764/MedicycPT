QMAKE_CXXFLAGS += -std=c++11
QT += core \
    gui \
    sql \
    network \
    testlib \
    widgets \
    charts \
    printsupport
CONFIG += qt warn_on depend_includepath testcase
    TEMPLATE = app
    TARGET = TestExternal
    HEADERS += TestQCPCurveUtils.h
    SOURCES += testmain.cpp TestQCPCurveUtils.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qcustomplot/release/ -lqcustomplot
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qcustomplot/debug/ -lqcustomplot
else:unix: LIBS += -L$$OUT_PWD/../qcustomplot/ -lqcustomplot

INCLUDEPATH += $$PWD/../qcustomplot
DEPENDPATH += $$PWD/../qcustomplot
