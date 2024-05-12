TEMPLATE = app
TARGET = MTD
QMAKE_CXXFLAGS += -std=c++11
QT += core \
    gui \
    sql \
    xml \
    network \
    opengl \
    widgets \
    charts \
    printsupport \
    dbus
HEADERS = MTD.h \
    TotalDoseGraph.h \
    ConfigDialog.h \
    DossierSelectionDialog.h \
    SeanceParameterSelectorDialog.h

SOURCES = MTD.cpp \
    TotalDoseGraph.cpp \
    main.cpp \
    ConfigDialog.cpp \
    DossierSelectionDialog.cpp \
    SeanceParameterSelectorDialog.cpp

FORMS = MTD.ui \
    DossierSelectionDialog.ui \
    SeanceParameterSelectorDialog.ui

DBUS_INTERFACES += ../DBus/RadiationMonitor.xml

win32: RESOURCES += ../Styles/breeze_win32.qrc
else:unix: RESOURCES += ../Styles/breeze_unix.qrc
RESOURCES += resources.qrc

LIBS += \
-L$$OUT_PWD/../DataStructures/protontherapy/src/ -lPTDataStructures \
-L$$OUT_PWD/../DataStructures/qualityassurance/src/ -lQADataStructures \
-L$$OUT_PWD/../Repository/protontherapy/src/ -lPTRepo \
-L$$OUT_PWD/../Graphs/src/ -lGraphs \
-L$$OUT_PWD/../Communications/ClientConnection/src -lClientConnection \
-L$$OUT_PWD/../Communications/NJ1/src -lNJ1 \
-L$$OUT_PWD/../Util/src/ -lUtil \
-L$$OUT_PWD/../External/qcustomplot/ -lqcustomplot \
-L$$OUT_PWD/../plugins/GenericGauge/ -lGenericGauge

INCLUDEPATH += \
$$PWD/../DataStructures/protontherapy/src \
$$PWD/../DataStructures/qualityassurance/src \
$$PWD/../Repository/protontherapy/src \
$$PWD/../Graphs/src \
$$PWD/../Util/src \
$$PWD/../Communications/ClientConnection/src \
$$PWD/../Communications/NJ1/src \
$$PWD/../External/qcustomplot \
$$PWD/../plugins/GenericGauge

DEPENDPATH += \
$$PWD/../DataStructures/protontherapy/src \
$$PWD/../Repository/protontherapy/src \
$$PWD/../Repository/qualityassurance/src \
$$PWD/../Graphs/src \
$$PWD/../Util/src \
$$PWD/../Communications/ClientConnection/src \
$$PWD/../Communications/NJ1/src \
$$PWD/../External/qcustomplot \
$$PWD/../plugins/GenericGauge
