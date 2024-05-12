TEMPLATE = app
TARGET = RadiationMonitor
QT += core \
    gui \
    network \
    sql \
    widgets \
    charts \
    dbus
HEADERS += RadiationMonitor.h RadiationLevelGraph.h
SOURCES += RadiationMonitor.cpp RadiationLevelGraph.cpp main.cpp

DBUS_INTERFACES += ../DBus/RadiationMonitor.xml

FORMS += RadiationMonitor.ui

unix: LIBS += \
-L$$OUT_PWD/../Communications/ClientConnection/src/ -lClientConnection \
-L$$OUT_PWD/../External/qcustomplot/ -lqcustomplot \
-L$$OUT_PWD/../Util/src/ -lUtil \
-L$$OUT_PWD/../DataStructures/protontherapy/src/ -lPTDataStructures

INCLUDEPATH += \
$$PWD/../Communications/ClientConnection/src \
$$PWD/../External/qcustomplot \
$$PWD/../Util/src \
$$PWD/../DataStructures/protontherapy/src

DEPENDPATH += \
$$PWD/../../Communications/ClientConnection/src \
$$PWD/../../Communications/NJ1/src \
$$PWD/../../External/qcustomplot \
$$PWD/../../Util/src \
$$PWD/../../DataStructures/protontherapy/src

RESOURCES += ../Styles/breeze_unix.qrc
