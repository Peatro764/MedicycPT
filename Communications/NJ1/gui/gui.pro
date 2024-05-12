TEMPLATE = app
TARGET = NJ1Gui
QT += core \
    gui \
    sql \
    network \
    widgets
HEADERS +=  NJ1Gui.h
SOURCES += NJ1Gui.cpp main.cpp

FORMS += NJ1Gui.ui

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


   
   
   
