TEMPLATE = lib
TARGET = Util
QMAKE_CXXFLAGS += -std=c++11
QT += core \
    gui
DEFINES += Util_LIBRARY
HEADERS +=  Calc.h Material.h Conversions.h TimedState.h ThreadSafeQueue.h

SOURCES += Calc.cpp Material.cpp Conversions.cpp TimedState.cpp

FORMS = 

RESOURCES +=
    

