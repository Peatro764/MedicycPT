TEMPLATE = lib
TARGET = PTDataStructures
QMAKE_CXXFLAGS += -std=c++11
QT += core \
    gui \
    sql \
    network \
    widgets \
    charts \
    printsupport
HEADERS += Chambre.h \
           Collimator.h \
           Compensateur.h \
           Coordinate.h \
           Modulateur.h \
           Debit.h \
           Degradeur.h \
           Dosimetry.h \
           DosimetryRecord.h \
           Patient.h \
           Seance.h \
           SeanceConfig.h \
           SeancePacket.h \
           SeanceRecord.h \
           BaliseCalibration.h \
           BaliseRecord.h \
           BaliseLevel.h \
           XRayRecord.h \
           Treatment.h \
           TreatmentType.h \
           TimedStampedDataSeries.h \
    Defaults.h \
    Util.h \
    Operator.h \
    DepthDoseCurve.h \
    DepthDose.h \
    SOBPMaker.h \
    ModulateurMaker.h \
    SOBPCurve.h \
    MCNPXData.h \
    Eye.h \
    ColorHandler.h \
    BaliseCalibration.h \
    Algorithms.h \
    VRDStatusMessage.h

SOURCES += Chambre.cpp \
           Collimator.cpp \
           Coordinate.cpp \
           Compensateur.cpp \
           Modulateur.cpp \
           Debit.cpp \
           Degradeur.cpp \
           Dosimetry.cpp \
           DosimetryRecord.cpp \
           Patient.cpp \
           Seance.cpp \
           SeanceConfig.cpp \
           SeancePacket.cpp \
           SeanceRecord.cpp \
           BaliseRecord.cpp \
           BaliseLevel.cpp \
           XRayRecord.cpp \
           Treatment.cpp \
           TreatmentType.cpp \
    Defaults.cpp \
    Util.cpp \
    DepthDoseCurve.cpp \
    DepthDose.cpp \
    SOBPMaker.cpp \
    ModulateurMaker.cpp \
    SOBPCurve.cpp \
    Eye.cpp \
    MCNPXData.cpp \
    ColorHandler.cpp \
    BaliseCalibration.cpp \
    Algorithms.cpp \
    VRDStatusMessage.cpp

FORMS = 

RESOURCES +=
    
DEFINES += PTDATASTRUCTURES_LIBRARY

win32:CONFIG(release, debug|release): LIBS += \
-L$$OUT_PWD/../../../External/qcustomplot/release/ -lqcustomplot \
-L$$OUT_PWD/../../../Util/src/release/ -lUtil
else:win32:CONFIG(debug, debug|release): LIBS += \
-L$$OUT_PWD/../../../External/qcustomplot/debug/ -lqcustomplot \
-L$$OUT_PWD/../../../Util/src/debug/ -lUtil
else:unix: LIBS += \
-L$$OUT_PWD/../../../External/qcustomplot/ -lqcustomplot \
-L$$OUT_PWD/../../../Util/src/ -lUtil

INCLUDEPATH += \
$$PWD/../../../External/qcustomplot \
$$PWD/../../../Util/src/
DEPENDPATH += \
$$PWD/../../../External/qcustomplot \
$$PWD/../../../Util/src/

