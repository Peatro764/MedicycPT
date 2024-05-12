QT += core network

TARGET = SocketIO
TEMPLATE = lib

DEFINES += NSINGLE_LIBRARY
SOURCES += SocketClient.cpp SocketServer.cpp
HEADERS += SocketClient.h SocketServer.h

