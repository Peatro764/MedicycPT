QT += core network serialport

TARGET = ClientConnection
TEMPLATE = lib

DEFINES += CLIENTCONNECTION_LIBRARY
SOURCES += SerialClient.cpp SocketClient.cpp Util.cpp
HEADERS += ClientConnection.h SerialClient.h SocketClient.h Util.h

