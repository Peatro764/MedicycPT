TEMPLATE = app
TARGET = TestGraphs
QT += core \
    gui \
    network \
    sql \
    widgets \
    charts
HEADERS +=  TestGraphs.h
SOURCES += TestGraphs.cpp main.cpp
FORMS += TestGraphs.ui


win32:CONFIG(release, debug|release): LIBS += \
-L$$OUT_PWD/../src/release/ -lGraphs \
-L$$OUT_PWD/../../External/qcustomplot/release/ -lqcustomplot \
-L$$OUT_PWD/../../DataStructures/protontherapy/src/release/ -lPTDatastructures \
-L$$OUT_PWD/../../DataStructures/qualityassurance/src/release/ -lQADatastructures \
-L$$OUT_PWD/../../Util/src/release/ -lUtil
else:win32:CONFIG(debug, debug|release): LIBS += \
-L$$OUT_PWD/../src/debug/ -lGraphs \
-L$$OUT_PWD/../../External/qcustomplot/debug/ -lqcustomplot \
-L$$OUT_PWD/../../DataStructures/protontherapy/src/debug/ -lPTDatastructures \
-L$$OUT_PWD/../../DataStructures/qualityassurance/src/debug/ -lQADatastructures \
-L$$OUT_PWD/../../Util/src/debug/ -lUtil
else:unix: LIBS += \
-L$$OUT_PWD/../src/ -lGraphs \
-L$$OUT_PWD/../../External/qcustomplot/ -lqcustomplot \
-L$$OUT_PWD/../../DataStructures/qualityassurance/src/ -lQADataStructures \
-L$$OUT_PWD/../../DataStructures/protontherapy/src/ -lPTDataStructures \
-L$$OUT_PWD/../../Util/src/ -lUtil

INCLUDEPATH += \
$$PWD/../src/ \
$$PWD/../../External/qcustomplot \
$$PWD/../../DataStructures/qualityassurance/src \
$$PWD/../../DataStructures/protontherapy/src \
$$PWD/../../Util/src

DEPENDPATH += \
$$PWD/../src/ \
$$PWD/../../External/qcustomplot \
$$PWD/../../DataStructures/qualityassurance/src \
$$PWD/../../DataStructures/protontherapy/src \
$$PWD/../../Util/src


