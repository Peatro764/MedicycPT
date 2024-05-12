QMAKE_CXXFLAGS += -std=c++11
QT += core \
    sql \
    network \
    testlib \
    charts \
    widgets
CONFIG += qt warn_on depend_includepath testcase
LIBS += -L/usr/local/lib
TEMPLATE = app
TARGET = TestQARepo
HEADERS += TestQARepo.h
SOURCES += TestQARepo.cpp testmain.cpp

win32:CONFIG(release, debug|release): LIBS += \
-L$$OUT_PWD/../../../DataStructures/qualityassurance/src/release/ -lQADataStructures \
-L$$OUT_PWD/../src/release/ -lQARepo \
-L$$OUT_PWD/../../../External/qcustomplot/release/ -lqcustomplot \
-L$$OUT_PWD/../../../Util/src/release -lUtil
else:win32:CONFIG(debug, debug|release): LIBS += \
 -L$$OUT_PWD/../../../DataStructures/qualityassurance/src/debug/ -lQADataStructures \
-L$$OUT_PWD/../src/debug/ -lQARepo \
-L$$OUT_PWD/../../../External/qcustomplot/debug/ -lqcustomplot \
-L$$OUT_PWD/../../../Util/src/debug -lUtil
else:unix: LIBS += \
-L$$OUT_PWD/../../../DataStructures/qualityassurance/src/ -lQADataStructures \
-L$$OUT_PWD/../src/ -lQARepo \
-L$$OUT_PWD/../../../External/qcustomplot/ -lqcustomplot \
-L$$OUT_PWD/../../../Util/src -lUtil

INCLUDEPATH += \
$$PWD/../../../DataStructures/qualityassurance/src $$PWD/../src \
$$PWD/../../../External/qcustomplot \
$$PWD/../../../Util/src
DEPENDPATH += \
$$PWD/../../../DataStructures/qualityassurance/src $$PWD/../src \
$$PWD/../../../External/qcustomplot \
$$PWD/../../../Util/src


RESOURCES = resources.qrc

