TEMPLATE = app
TARGET = SocketIOGui
QT += core \
    gui \
    network \
    widgets
HEADERS +=  SocketIOGui.h
SOURCES += SocketIOGui.cpp main.cpp

FORMS += SocketIOGui.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../src/release/ -lSocketIO
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../src/debug/ -lSocketIO
else:unix: LIBS += -L$$OUT_PWD/../src/ -lSocketIO

INCLUDEPATH += $$PWD/../src
DEPENDPATH += $$PWD/../src
