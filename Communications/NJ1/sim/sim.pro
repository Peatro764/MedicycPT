TEMPLATE = app
TARGET = NJ1Sim
QT += core \
    gui \
    sql \
    network \
    widgets
HEADERS +=  NJ1Sim.h
SOURCES += NJ1Sim.cpp main.cpp

FORMS += NJ1Sim.ui

LIBS += \
-L$$OUT_PWD/../src/ -lNJ1 \
-L$$OUT_PWD/../../ClientConnection/src/ -lClientConnection \
-L$$OUT_PWD/../../../DataStructures/protontherapy/src/ -lPTDataStructures \
-L$$OUT_PWD/../../../Util/src/ -lUtil \
-L$$OUT_PWD/../../../External/qcustomplot/ -lqcustomplot


INCLUDEPATH += \
$$PWD/../src \
$$PWD/../../ClientConnection/src \
$$PWD/../../../DataStructures/protontherapy/src \
$$PWD/../../../Util/src \
$$PWD/../../../External/qcustomplot

DEPENDPATH += \
$$PWD/../src \
$$PWD/../../ClientConnection/src \
$$PWD/../../../DataStructures/protontherapy/src \
$$PWD/../../../Util/src \
$$PWD/../../../External/qcustomplot


   
   
   
