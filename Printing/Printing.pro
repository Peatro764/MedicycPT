TEMPLATE = lib
TARGET = Printing
QT += core \
    gui \
    network \
    sql \
    widgets \
    charts \
    printsupport
HEADERS +=  TreatmentPrintDialog.h ControleQualitePrintDialog.h ControleDeLaDosePrintDialog.h CollimatorPrintDialog.h ProfilePrintDialog.h BraggPeakPrintDialog.h SOBPPrintDialog.h TopDeJourPrintDialog.h PatientDebitPrintDialog.h
SOURCES +=  TreatmentPrintDialog.cpp ControleQualitePrintDialog.cpp ControleDeLaDosePrintDialog.cpp CollimatorPrintDialog.cpp ProfilePrintDialog.cpp BraggPeakPrintDialog.cpp SOBPPrintDialog.cpp TopDeJourPrintDialog.cpp PatientDebitPrintDialog.cpp
FORMS += TreatmentPrintDialog.ui ControleQualitePrintDialog.ui ControleDeLaDosePrintDialog.ui CollimatorPrintDialog.ui ProfilePrintDialog.ui BraggPeakPrintDialog.ui SOBPPrintDialog.ui TopDeJourPrintDialog.ui PatientDebitPrintDialog.ui
win32:CONFIG(release, debug|release): LIBS += \
-L$$OUT_PWD/../Graphs/src/release/ -lGraphs \
-L$$OUT_PWD/../External/qcustomplot/release/ -lqcustomplot \
-L$$OUT_PWD/../Repository/qualityassurance/src/release/ -lQARepo \
-L$$OUT_PWD/../Repository/protontherapy/src/release/ -lPTRepo \
-L$$OUT_PWD/../DataStructures/qualityassurance/src/release/ -lQADataStructures \
-L$$OUT_PWD/../DataStructures/protontherapy/src/release/ -lPTDataStructures \
-L$$OUT_PWD/../Util/src/release/ -lUtil
else:win32:CONFIG(debug, debug|release): LIBS += \
-L$$OUT_PWD/../Graphs/src/debug/ -lGraphs \
-L$$OUT_PWD/../External/qcustomplot/debug/ -lqcustomplot \
-L$$OUT_PWD/../Repository/qualityassurance/src/debug/ -lQARepo \
-L$$OUT_PWD/../Repository/protontherapy/src/debug/ -lPTRepo \
-L$$OUT_PWD/../DataStructures/qualityassurance/src/debug/ -lQADataStructures \
-L$$OUT_PWD/../DataStructures/protontherapy/src/debug/ -lPTDataStructures \
-L$$OUT_PWD/../Util/src/debug/ -lUtil
else:unix: LIBS += \
-L$$OUT_PWD/../Graphs/src/ -lGraphs \
-L$$OUT_PWD/../External/qcustomplot/ -lqcustomplot \
-L$$OUT_PWD/../Repository/qualityassurance/src/ -lQARepo \
-L$$OUT_PWD/../Repository/protontherapy/src/ -lPTRepo \
-L$$OUT_PWD/../DataStructures/qualityassurance/src/ -lQADataStructures \
-L$$OUT_PWD/../DataStructures/protontherapy/src/ -lPTDataStructures \
-L$$OUT_PWD/../Util/src/ -lUtil

INCLUDEPATH += \
$$PWD/../Graphs/src/ \
$$PWD/../External/qcustomplot \
$$PWD/../DataStructures/qualityassurance/src \
$$PWD/../DataStructures/protontherapy/src \
$$PWD/../Util/src \
$$PWD/../Repository/protontherapy/src \
$$PWD/../Repository/qualityassurance/src

DEPENDPATH += \
$$PWD/../Graphs/src/ \
$$PWD/../External/qcustomplot \
$$PWD/../DataStructures/qualityassurance/src \
$$PWD/../Repository/qualityassurance/src \
$$PWD/../Repository/protontherapy/src \
$$PWD/../Util/src
