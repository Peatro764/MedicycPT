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
    HEADERS += TestCalc.h TestMaterial.h TestConversions.h
    SOURCES += TestCalc.cpp TestMaterial.cpp TestConversions.cpp testmain.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../src/release/ -lUtil
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../src/debug/ -lUtil
else:unix: LIBS += -L$$OUT_PWD/../src/ -lUtil

INCLUDEPATH += $$PWD/../src
DEPENDPATH += $$PWD/../src
