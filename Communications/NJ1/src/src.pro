QT += core network

TARGET = NJ1
TEMPLATE = lib
QMAKE_CXXFLAGS += -std=c++11

DEFINES += NJ1_LIBRARY
SOURCES += NJ1.cpp
HEADERS += NJ1.h

LIBS += \
-L$$OUT_PWD/../../ClientConnection/src/ -lClientConnection \
-L$$OUT_PWD/../../../DataStructures/protontherapy/src/ -lPTDataStructures \
-L$$OUT_PWD/../../../Util/src/ -lUtil \
-L$$OUT_PWD/../../../External/qcustomplot/ -lqcustomplot

INCLUDEPATH += \
$$PWD/../../ClientConnection/src \
$$PWD/../../../DataStructures/protontherapy/src \
$$PWD/../../../Util/src \
$$PWD/../../../External/qcustomplot

DEPENDPATH += \
$$PWD/../../ClientConnection/src \
$$PWD/../../../DataStructures/protontherapy/src \
$$PWD/../../../Util/src \
$$PWD/../../../External/qcustomplot

