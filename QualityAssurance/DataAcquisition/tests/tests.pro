QMAKE_CXXFLAGS += -std=c++11
QT += core \
    gui \
    network \
    sql \
    testlib \
    widgets
CONFIG += qt warn_on depend_includepath testcase
LIBS += -L/usr/local/lib
TEMPLATE = app
TARGET = TestDataAcquisition
HEADERS += TestDeviceManager.h
SOURCES += TestDeviceManager.cpp testmain.cpp

win32:CONFIG(release, debug|release): LIBS += \
-L$$OUT_PWD/../src/release/ -lDataAcquisition -lLabJackM \
-L$$OUT_PWD/../../../External/qcustomplot/release/ -lqcustomplot \
-L$$OUT_PWD/../../../DataStructures/release/ -lDataStructures \
-L$$OUT_PWD/../../../Repository/release/ -lRepo
else:win32:CONFIG(debug, debug|release): LIBS += \
-L$$OUT_PWD/../src/debug/ -lDataAcquisition -lLabJackM \
-L$$OUT_PWD/../../../External/qcustomplot/debug/ -lqcustomplot \
-L$$OUT_PWD/../../../DataStructures/src/debug/ -lDataStructures \
-L$$OUT_PWD/../../../Repository/src/debug/ -lRepo
else:unix: LIBS += \
-L$$OUT_PWD/../src/ -lDataAcquisition -lLabJackM \
-L$$OUT_PWD/../../../External/qcustomplot/ -lqcustomplot \
-L$$OUT_PWD/../../../DataStructures/src/ -lDataStructures \
-L$$OUT_PWD/../../../Repository/src/ -lRepo

INCLUDEPATH += \
$$PWD/../src/ \
$$PWD/../../../External/qcustomplot \
$$PWD/../../../DataStructures/src \
$$PWD/../../../Repository/src

DEPENDPATH += \
$$PWD/../src \
$$PWD/../../../External/qcustomplot
$$PWD/../../../DataStructures/src
$$PWD/../../../Repositorty/src
