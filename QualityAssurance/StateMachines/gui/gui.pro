TEMPLATE = app
TARGET = StateMachinesGui
QT += core \
    gui \
    network \
    sql \
    widgets \
    charts
HEADERS +=  StateMachinesGui.h Histogram.h
SOURCES += StateMachinesGui.cpp main.cpp Histogram.cpp
FORMS += StateMachinesGui.ui

LIBS += \
-L$$OUT_PWD/../src/ -lStateMachines \
-L$$OUT_PWD/../../DataAcquisition/src/ -lDataAcquisition -lLabJackM \
-L$$OUT_PWD/../../../External/qcustomplot/ -lqcustomplot \
-L$$OUT_PWD/../../../DataStructures/qualityassurance/src/ -lQADataStructures \
-L$$OUT_PWD/../../../Repository/qualityassurance/src/ -lQARepo \
-L$$OUT_PWD/../../../Util/src/ -lUtil

INCLUDEPATH += \
$$PWD/../src/ \
$$PWD/../../DataAcquisition/src \
$$PWD/../../../External/qcustomplot \
$$PWD/../../../Repository/qualityassurance/src \
$$PWD/../../../DataStructures/qualityassurance/src \
$$PWD/../../../Util/src

DEPENDPATH += \
$$PWD/../src/ \
$$PWD/../../DataAcquisition/src \
$$PWD/../../../External/qcustomplot \
$$PWD/../../../Repository/qualityassurance/src \
$$PWD/../../../DataStructures/qualityassurance/src \
$$PWD/../../../Util/src
