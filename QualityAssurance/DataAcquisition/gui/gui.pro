TEMPLATE = app
TARGET = DataAcquisitionGui
QT += core \
    gui \
    network \
    sql \
    widgets \
    charts
HEADERS +=  DataAcquisitionGui.h
SOURCES += DataAcquisitionGui.cpp main.cpp
FORMS += DataAcquisitionGui.ui

win32:CONFIG(release, debug|release): LIBS += \
-L$$OUT_PWD/../src/release/ -lDataAcquisition -lLabJackM \
-L$$OUT_PWD/../../../External/qcustomplot/release/ -lqcustomplot \
-L$$OUT_PWD/../../../DataStructures/qualityassurance/release/ -lQADataStructures \
-L$$OUT_PWD/../../../Repository/qualityassurance/release/ -lQARepo \
-L$$OUT_PWD/../../../Util/release/ -lUtil
else:win32:CONFIG(debug, debug|release): LIBS += \
-L$$OUT_PWD/../src/debug/ -lDataAcquisition -lLabJackM \
-L$$OUT_PWD/../../../External/qcustomplot/debug/ -lqcustomplot \
-L$$OUT_PWD/../../../DataStructures/qualityassurance/src/debug/ -lQADataStructures \
-L$$OUT_PWD/../../../Repository/qualityassurance/src/debug/ -lQARepo \
-L$$OUT_PWD/../../../Util/src/debug/ -lUtil
else:unix: LIBS += \
-L$$OUT_PWD/../src/ -lDataAcquisition -lLabJackM \
-L$$OUT_PWD/../../../External/qcustomplot/ -lqcustomplot \
-L$$OUT_PWD/../../../DataStructures/qualityassurance/src/ -lQADataStructures \
-L$$OUT_PWD/../../../Repository/qualityassurance/src/ -lQARepo \
-L$$OUT_PWD/../../../Util/src/ -lUtil

INCLUDEPATH += \
$$PWD/../src/ \
$$PWD/../../../External/qcustomplot \
$$PWD/../../../DataStructures/qualityassurance/src \
$$PWD/../../../Repository/qualityassurance/src \
$$PWD/../../../Util/src

DEPENDPATH += \
$$PWD/../src \
$$PWD/../../../External/qcustomplot \
$$PWD/../../../DataStructures/qualityassurance/src \
$$PWD/../../../Repository/qualityassurance/src \
$$PWD/../../../Util/src

RESOURCES += resources.qrc
