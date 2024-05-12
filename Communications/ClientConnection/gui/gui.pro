TEMPLATE = app
TARGET = ClientConnectionGui
QT += core \
    gui \
    network \
    serialport \
    widgets
HEADERS +=  ClientConnectionGui.h
SOURCES += ClientConnectionGui.cpp main.cpp

FORMS += ClientConnectionGui.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../src/release/ -lClientConnection
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../src/debug/ -lClientConnection
else:unix: LIBS += -L$$OUT_PWD/../src/ -lClientConnection

INCLUDEPATH += $$PWD/../src
DEPENDPATH += $$PWD/../src
