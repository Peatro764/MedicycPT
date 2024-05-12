TEMPLATE = lib
TARGET = QADataStructures
QMAKE_CXXFLAGS += -std=c++11
QT += core \
    gui \
    sql \
    network \
    widgets \
    charts \
    printsupport
HEADERS += Point.h \
           Axis.h \
           BeamSignal.h \
           Range.h \
           Beacon.h \
           MeasurementPoint.h \
           DepthDoseMeasurement.h \
           BeamMeasurement.h \
           BraggPeak.h \
           BraggPeakSeries.h \
           SOBPSeries.h \
           BeamProfile.h \
           BeamProfileResults.h \
           BeamProfileSeries.h \
           MeasurementCurrents.h \
           CuveCube.h \
           DepthDoseResults.h \
           Hardware.h \
           LabJackChannels.h \
           SOBP.h \
           SensorConfig.h \
           MotorConfig.h \
           ClockConfig.h


SOURCES += Point.cpp \
           BeamSignal.cpp \
           Axis.cpp \
           Range.cpp \
           MeasurementPoint.cpp \
           BeamProfile.cpp \
           BraggPeak.cpp \
           DepthDoseMeasurement.cpp \
           BraggPeakSeries.cpp \
           SOBPSeries.cpp \
           BeamMeasurement.cpp \
           BeamProfileSeries.cpp \
           CuveCube.cpp \
           Hardware.cpp \
           SensorConfig.cpp \
           SOBP.cpp \
           MeasurementCurrents.cpp

FORMS = 

RESOURCES +=
    
DEFINES += QADATASTRUCTURES_LIBRARY

win32:CONFIG(release, debug|release): LIBS += \
-L$$OUT_PWD/..//../../External/qcustomplot/release/ -lqcustomplot \
-L$$OUT_PWD/../../../Util/src/release/ -lUtil
else:win32:CONFIG(debug, debug|release): LIBS += \
-L$$OUT_PWD/../../../External/qcustomplot/debug/ -lqcustomplot \
-L$$OUT_PWD/../../../Util/src/debug/ -lUtil
else:unix: LIBS += \
-L$$OUT_PWD/../../../External/qcustomplot/ -lqcustomplot \
-L$$OUT_PWD/../../../Util/src/ -lUtil


INCLUDEPATH += \
$$PWD/../../../External/qcustomplot \
$$PWD/../../../Util/src

DEPENDPATH += \
$$PWD/../../../External/qcustomplot \
$$PWD/../../../Util/src
