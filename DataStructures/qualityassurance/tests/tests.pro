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
    TARGET = TestQADataStructures
    HEADERS += TestCuveCube.h TestDepthDoseMeasurement.h TestSOBP.h TestBraggPeak.h TestBeamProfile.h TestBeamMeasurement.h TestMeasurementPoint.h
    SOURCES += TestCuveCube.cpp TestDepthDoseMeasurement.cpp TestSOBP.cpp TestBraggPeak.cpp testmain.cpp TestBeamProfile.cpp TestBeamMeasurement.cpp TestMeasurementPoint.cpp

win32:CONFIG(release, debug|release): LIBS += \
-L$$OUT_PWD/../../../External/qcustomplot/release/ -lqcustomplot \
-L$$OUT_PWD/../src/release/ -lQADataStructures \
-L$$OUT_PWD/../../../Util/src/release/ -lUtil
else:win32:CONFIG(debug, debug|release): LIBS += \
-L$$OUT_PWD/../../../External/qcustomplot/debug/ -lqcustomplot \
-L$$OUT_PWD/../src/debug/ -lQADataStructures \
-L$$OUT_PWD/../../../Util/src/debug/ -lUtil
else:unix: LIBS += \
-L$$OUT_PWD/../../../External/qcustomplot/ -lqcustomplot \
-L$$OUT_PWD/../src/ -lQADataStructures \
-L$$OUT_PWD/../../../Util/src/ -lUtil

INCLUDEPATH += \
$$PWD/../src \
$$PWD/../../../External/qcustomplot \
$$PWD/../../../Util/src

DEPENDPATH += \
$$PWD/../src \
$$PWD/../../../External/qcustomplot
$$PWD/../../../Util/src

RESOURCES = resources.qrc
