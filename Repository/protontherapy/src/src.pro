TEMPLATE = lib
TARGET = PTRepo
QMAKE_CXXFLAGS += -std=c++11
QT += core \
    sql \
    charts
HEADERS = PTRepo.h DbUtil.h
SOURCES = PTRepo.cpp DbUtil.cpp
    
win32:CONFIG(release, debug|release): LIBS += \
-L$$OUT_PWD/../../../External/qcustomplot/release/ -lqcustomplot \
-L$$OUT_PWD/../../../DataStructures/protontherapy/src/release/ -lPTDataStructures \
-L$$OUT_PWD/../../../Util/src/release/ -lUtil
else:win32:CONFIG(debug, debug|release): LIBS += \
-L$$OUT_PWD/../../../External/qcustomplot/debug/ -lqcustomplot \
-L$$OUT_PWD/../../../DataStructures/protontherapy/src/debug/ -lPTDataStructures \
-L$$OUT_PWD/../../../Util/src/debug/ -lUtil
else:unix: LIBS += \
-L$$OUT_PWD/../../../External/qcustomplot/ -lqcustomplot \
-L$$OUT_PWD/../../../DataStructures/protontherapy/src/ -lPTDataStructures \
-L$$OUT_PWD/../../../Util/src/ -lUtil

INCLUDEPATH += \
$$PWD/../../../DataStructures/protontherapy/src \
$$PWD/../../../External/qcustomplot \
$$PWD/../../../Util/src
DEPENDPATH += \
$$PWD/../../../DataStructures/protontherapy/src \
$$PWD/../../../External/qcustomplot \
$$PWD/../../../Util/src
