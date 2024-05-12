TEMPLATE = app
TARGET = MQA
QT += core \
    gui \
    network \
    sql \
    widgets \
    charts \
    printsupport \
    dbus
HEADERS +=  MQA.h BeamLineGui.h DossierSelectionDialog.h SeanceParameterSelectorDialog.h SOBPResultsDialog.h PrintDialog.h CuveCustomPlot.h ShowSOBPDialog.h DepthDoseTypeDialog.h TopDeJourGui.h PatientDebitGui.h DoseRateSettingsDialog.h DoseRateMeasurementDialog.h CSVDialog.h ManualMotorControlDialog.h GuiBase.h Scanner3DGui.h FetchSOBPDialog.h FetchScanner3DDialog.h SaveScanner3DDialog.h BraggPeakGui.h ShowBraggPeakDialog.h BeamProfilerGui.h BraggPeakHistory.h SOBPHistory.h BeamProfilerHistory.h ShowBeamProfileDialog.h ConfigDialog.h
SOURCES += MQA.cpp BeamLineGui.cpp DossierSelectionDialog.cpp SeanceParameterSelectorDialog.cpp SOBPResultsDialog.cpp PrintDialog.cpp CuveCustomPlot.cpp ShowSOBPDialog.cpp DepthDoseTypeDialog.cpp TopDeJourGui.cpp PatientDebitGui.cpp DoseRateSettingsDialog.cpp DoseRateMeasurementDialog.cpp CSVDialog.cpp ManualMotorControlDialog.cpp GuiBase.cpp Scanner3DGui.cpp FetchSOBPDialog.cpp FetchScanner3DDialog.cpp SaveScanner3DDialog.cpp BraggPeakGui.cpp ShowBraggPeakDialog.cpp BeamProfilerGui.cpp BraggPeakHistory.cpp SOBPHistory.cpp BeamProfilerHistory.cpp ShowBeamProfileDialog.cpp ConfigDialog.cpp main.cpp
FORMS += MQA.ui DossierSelectionDialog.ui SeanceParameterSelectorDialog.ui SOBPResultsDialog.ui PrintDialog.ui ShowSOBPDialog.ui DepthDoseTypeDialog.ui DoseRateSettingsDialog.ui CSVDialog.ui ManualMotorControlDialog.ui FetchSOBPDialog.ui FetchScanner3DDialog.ui SaveScanner3DDialog.ui ShowBeamProfileDialog.ui ShowBraggPeakDialog.ui

DBUS_INTERFACES += ../../DBus/RadiationMonitor.xml

LIBS += \
-L$$OUT_PWD/../../DataStructures/protontherapy/src -lPTDataStructures \
-L$$OUT_PWD/../../Printing/ -lPrinting \
-L$$OUT_PWD/../StateMachines/src/ -lStateMachines \
-L$$OUT_PWD/../DataAcquisition/src/ -lDataAcquisition -lLabJackM \
-L$$OUT_PWD/../../External/qcustomplot/ -lqcustomplot \
-L$$OUT_PWD/../../DataStructures/qualityassurance/src/ -lQADataStructures \
-L$$OUT_PWD/../../Repository/qualityassurance/src/ -lQARepo \
-L$$OUT_PWD/../../Repository/protontherapy/src/ -lPTRepo \
-L$$OUT_PWD/../../Util/src/ -lUtil \
-L$$OUT_PWD/../../Graphs/src -lGraphs \
-L$$OUT_PWD/../../Communications/ClientConnection/src -lClientConnection \
-L$$OUT_PWD/../../Communications/NJ1/src -lNJ1

INCLUDEPATH += \
$$PWD/../../Graphs/src \
$$PWD/../../DataStructures/protontherapy/src/ \
$$PWD/../../Printing/ \
$$PWD/../StateMachines/src/ \
$$PWD/../DataAcquisition/src/ \
$$PWD/../../External/qcustomplot \
$$PWD/../../DataStructures/qualityassurance/src \
$$PWD/../../Repository/qualityassurance/src \
$$PWD/../../Repository/protontherapy/src \
$$PWD/../../Util/src \
$$PWD/../../Communications/ClientConnection/src \
$$PWD/../../Communications/NJ1/src

DEPENDPATH += \
$$PWD/../../Graphs/src/ \
$$PWD/../../Printing/ \
$$PWD/../../DataStructures/protontherapy/src/ \
$$PWD/../StateMachines/src \
$$PWD/../DataAcquisition/src/ \
$$PWD/../../External/qcustomplot \
$$PWD/../../DataStructures/qualityassurance/src \
$$PWD/../../Repository/qualityassurance/src \
$$PWD/../../Util/src \
$$PWD/../../Communications/ClientConnection/src \
$$PWD/../../Communications/NJ1/src

win32: RESOURCES += ../../Styles/breeze_win32.qrc
else:unix: RESOURCES += ../../Styles/breeze_unix.qrc

RESOURCES += resources.qrc
