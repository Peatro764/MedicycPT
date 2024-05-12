#QT       -= gui
QT       += serialport network

TARGET = MillingMachine
TEMPLATE = lib

DEFINES += MillingMachine_LIBRARY

SOURCES += MillingMachine.cpp \
    SerialPortWriter.cpp \
    SerialPortReader.cpp \
    SocketWriter.cpp \
    SocketReader.cpp

HEADERS += MillingMachine.h\
        millingmachine_global.h \
    SerialPortWriter.h \
    SerialPortReader.h \
    SocketWriter.h \
    SocketReader.h \
    DataReader.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../DataStructures/protontherapy/src/release/ -lPTDataStructures
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../DataStructures/protontherapy/src/debug/ -lPTDataStructures
else:unix: LIBS += -L$$OUT_PWD/../../DataStructures/protontherapy/src/ -lPTDataStructures

INCLUDEPATH += $$PWD/../../DataStructures/protontherapy/src/
DEPENDPATH += $$PWD/../../DataStructures/protontherapy/src/

win32:CONFIG(release, debug|release): LIBS += \
-L$$OUT_PWD/../../External/qcustomplot/release/ -lqcustomplot
else:win32:CONFIG(debug, debug|release): LIBS += \
-L$$OUT_PWD/../../External/qcustomplot/debug/ -lqcustomplot
else:unix: LIBS += \
-L$$OUT_PWD/../../External/qcustomplot/ -lqcustomplot

INCLUDEPATH += $$PWD/../../External/qcustomplot
DEPENDPATH += $$PWD/../../External/qcustomplot
