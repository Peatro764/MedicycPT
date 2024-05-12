QT += core \
      network \
      sql
TARGET = StateMachines
TEMPLATE = lib

DEFINES += STATEMACHINES_LIBRARY
SOURCES += Scanner3D.cpp BeamProfiler.cpp BraggPeaker.cpp AxisScan.cpp AxisStepper.cpp
HEADERS += Scanner3D.h BeamProfiler.h BraggPeaker.h AxisScan.h AxisStepper.h

win32:CONFIG(release, debug|release): LIBS += \
-L$$OUT_PWD/../../DataAcquisition/src/release/ -lDataAcquisition -lLabJackM \
-L$$OUT_PWD/../../../DataStructures/qualityassurance/release/ -lQADataStructures
else:win32:CONFIG(debug, debug|release): LIBS += \
-L$$OUT_PWD/../../DataAcquisition/src/release/ -lDataAcquisition -lLabJackM \
-L$$OUT_PWD/../../../DataStructures/qualityassurance/src/debug/ -lQADataStructures
else:unix: LIBS += \
-L$$OUT_PWD/../../DataAcquisition/src/ -lDataAcquisition -lLabJackM \
-L$$OUT_PWD/../../../DataStructures/qualityassurance/src/ -lQADataStructures \
-L$$OUT_PWD/../../../External/qcustomplot/ -lqcustomplot \
-L$$OUT_PWD/../../../Util/src/ -lUtil

INCLUDEPATH += \
$$PWD/../../DataAcquisition/src/ \
$$PWD/../../../External/qcustomplot \
$$PWD/../../../DataStructures/qualityassurance/src \
$$PWD/../../../Repository/qualityassurance/src \
$$PWD/../../../Util/src

   DEPENDPATH += \
$$PWD/../../DataAcquisition/src \
$$PWD/../../../External/qcustomplot \
$$PWD/../../../DataStructures/qualityassurance/src \
$$PWD/../../../Repository/qualityassurance/src \
$$PWD/../../../Util/src
