QMAKE_CXXFLAGS += -std=c++11
QT += core \
    gui \
    sql \
    network \
    testlib \
    widgets \
    charts \
    printsupport
CONFIG += qt warn_on depend_includepath testcase
    TEMPLATE = app
    TARGET = TestPTDataStructures
    HEADERS += TestTreatmentType.h TestTreatment.h TestDosimetry.h TestDosimetryRecord.h TestSeance.h TestSeanceRecord.h TestPatient.h TestUtil.h TestDegradeur.h TestModulateur.h TestCollimateur.h TestDepthDose.h TestDepthDoseCurve.h \
    TestSOBPCurve.h TestSOBPMaker.h TestEye.h TestCompensateur.h TestModulateurMaker.h TestColorHandler.h TestAlgorithms.h TestSeanceConfig.h TestSeancePacket.h

    SOURCES += TestTreatmentType.cpp TestTreatment.cpp TestDosimetry.cpp TestDosimetryRecord.cpp TestSeance.cpp TestSeanceRecord.cpp TestPatient.cpp TestUtil.cpp TestDegradeur.cpp TestModulateur.cpp TestCollimateur.cpp TestDepthDose.cpp TestDepthDoseCurve.cpp testmain.cpp \
    TestSOBPCurve.cpp TestSOBPMaker.cpp TestEye.cpp TestCompensateur.cpp TestModulateurMaker.cpp TestColorHandler.cpp TestAlgorithms.cpp TestSeanceConfig.cpp TestSeancePacket.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../src/release/ -lPTDataStructures
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../src/debug/ -lPTDataStructures
else:unix: LIBS += -L$$OUT_PWD/../src/ -lPTDataStructures

INCLUDEPATH += $$PWD/../src
DEPENDPATH += $$PWD/../src

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

RESOURCES += resources.qrc
