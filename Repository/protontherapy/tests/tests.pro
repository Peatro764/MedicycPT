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
TARGET = TestPTRepo
HEADERS += TestPTRepo.h TestDbUtil.h
SOURCES += TestPTRepo.cpp TestDbUtil.cpp testmain.cpp

win32:CONFIG(release, debug|release): LIBS += \
-L$$OUT_PWD/../../../DataStructures/protontherapy/src/release/ -lPTDataStructures \
-L$$OUT_PWD/../../../Util/src/release/ -lUtil \
-L$$OUT_PWD/../../../External/qcustomplot/release/ -lqcustomplot \
-L$$OUT_PWD/../src/release/ -lPTRepo

else:win32:CONFIG(debug, debug|release): LIBS += \
 -L$$OUT_PWD/../../../DataStructures/protontherapy/src/debug/ -lPTDataStructures \
 -L$$OUT_PWD/../../../External/qcustomplot/debug/ -lqcustomplot \
 -L$$OUT_PWD/../../../Util/src/debug/ -lUtil \
 -L$$OUT_PWD/../src/debug/ -lPTRepo

else:unix: LIBS += \
-L$$OUT_PWD/../../../DataStructures/protontherapy/src/ -lPTDataStructures \
-L$$OUT_PWD/../../../Util/src/ -lUtil \
-L$$OUT_PWD/../../../External/qcustomplot/ -lqcustomplot \
-L$$OUT_PWD/../src/ -lPTRepo

INCLUDEPATH += \
$$PWD/../../../DataStructures/protontherapy/src $$PWD/../src \
$$PWD/../../../External/qcustomplot \
$$PWD/../../../Util/src
DEPENDPATH += \
$$PWD/../../../DataStructures/protontherapy/src $$PWD/../src \
$$PWD/../../../External/qcustomplot \
$$PWD/../../../Util/src

RESOURCES = resources.qrc

