TEMPLATE = lib
TARGET = QARepo
QMAKE_CXXFLAGS += -std=c++11
QT += core \
    sql \
    charts
HEADERS = QARepo.h
SOURCES = QARepo.cpp

FORMS =

RESOURCES +=

DEFINES += QAREPO_LIBRARY

win32:CONFIG(release, debug|release): LIBS += \
-L$$OUT_PWD/../../../External/qcustomplot/release/ -lqcustomplot \
-L$$OUT_PWD/../../../DataStructures/qualityassurance/src/release/ -lQADataStructures \
-L$$OUT_PWD/../../../Util/src/release/ -lUtil
else:win32:CONFIG(debug, debug|release): LIBS += \
-L$$OUT_PWD/../../../External/qcustomplot/debug/ -lqcustomplot \
-L$$OUT_PWD/../../../DataStructures/qualityassurance/src/debug/ -lQADataStructures \
-L$$OUT_PWD/../../../Util/src/debug/ -lUtil
else:unix: LIBS += \
-L$$OUT_PWD/../../../DataStructures/qualityassurance/src/ -lQADataStructures \
-L$$OUT_PWD/../../../External/qcustomplot/ -lqcustomplot \
-L$$OUT_PWD/../../../Util/src/ -lqcustomplot

INCLUDEPATH += \
$$PWD/../../../DataStructures/qualityassurance/src \
$$PWD/../../../External/qcustomplot \
$$PWD/../../../Util/src

DEPENDPATH += \
$$PWD/../../../DataStructures/qualityassurance/src \
$$PWD/../../../External/qcustomplot \
$$PWD/../../../Util/src



