QMAKE_CXXFLAGS += -std=c++11
QT += core \
    gui \
    sql \
    network \
    testlib \
    widgets
CONFIG += qt warn_on depend_includepath testcase
    TEMPLATE = app
    TARGET = TestCollimator
    HEADERS += TestCollimator.h
    SOURCES += TestCollimator.cpp testmain.cpp

win32:CONFIG(release, debug|release): LIBS += \
-L$$OUT_PWD/../src/release/ -lMillingMachine \
-L$$OUT_PWD/../../DataStructures/protontherapy/src/release/ -lPTDataStructures \
-L$$OUT_PWD/../../External/qcustomplot/release/ -lqcustomplot \
-L$$OUT_PWD/../../Util/src/release/ -lUtil
else:win32:CONFIG(debug, debug|release): LIBS += \
-L$$OUT_PWD/../src/debug/ -lMillingMachine \
-L$$OUT_PWD/../../../DataStructures/protontherapy/src/debug/ -lPTDataStructures \
-L$$OUT_PWD/../../External/qcustomplot/debug/ -lqcustomplot \
-L$$OUT_PWD/../../Util/src/debug/ -lUtil
else:unix: LIBS += \
-L$$OUT_PWD/../src/ -lMillingMachine \
-L$$OUT_PWD/../../DataStructures/protontherapy/src/ -lPTDataStructures \
-L$$OUT_PWD/../../External/qcustomplot/ -lqcustomplot \
-L$$OUT_PWD/../../Util/src -lUtil

INCLUDEPATH += \
$$PWD/../src \
$$PWD/../../DataStructures/protontherapy/src/ \
$$PWD/../../External/qcustomplot \
$$PWD/../../Util/src
DEPENDPATH += \
$$PWD/../src \
$$PWD/../../DataStructures/protontherapy/src/ \
$$PWD/../../External/qcustomplot \
$$PWD/../../Util/src

RESOURCES = resources.qrc
