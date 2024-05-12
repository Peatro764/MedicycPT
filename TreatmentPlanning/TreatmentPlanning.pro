TEMPLATE = app
TARGET = MTP
QMAKE_CXXFLAGS += -std=c++11
QT += core \
    gui \
    sql \
    network \
    widgets \
    charts \
    printsupport
HEADERS +=  MTP.h ChangeTreatmentTypeDialog.h CommentsDialog.h LibraryPage.h DossierPage.h CollimateurPage.h CompensateurPage.h ConfigDialog.h \
    CompXZGraph.h CompXYGraph.h EyeXZGraph.h CompErrorGraph.h MillingMachineProgramDialog.h LogDialog.h ChangeModulateurDialog.h ChangeDegradeurDialog.h \
    AddSeanceDialog.h ModifyPatientNameDialog.h ModifyDeliveredDoseDialog.h CreateDossierDialog.h ShowDeliveredDoseDialog.h ModulateurPage.h \
    MathUtilsPage.h DeleteSeanceDialog.h AssociateDialog.h ModifyPlannedDoseDialog.h \
    PrintDialog.h \
    SelectPatientDialog.h
SOURCES += MTP.cpp ChangeTreatmentTypeDialog.cpp CommentsDialog.cpp DossierPage.cpp LibraryPage.cpp CollimateurPage.cpp CompensateurPage.cpp main.cpp ConfigDialog.cpp \
    CompXZGraph.cpp CompXYGraph.cpp EyeXZGraph.cpp CompErrorGraph.cpp MillingMachineProgramDialog.cpp LogDialog.cpp ChangeModulateurDialog.cpp ChangeDegradeurDialog.cpp \
    AddSeanceDialog.cpp ModifyPatientNameDialog.cpp ModifyDeliveredDoseDialog.cpp CreateDossierDialog.cpp ShowDeliveredDoseDialog.cpp ModulateurPage.cpp \
    MathUtilsPage.cpp DeleteSeanceDialog.cpp AssociateDialog.cpp ModifyPlannedDoseDialog.cpp \
    PrintDialog.cpp \
    SelectPatientDialog.cpp
FORMS += MTP.ui ChangeTreatmentTypeDialog.ui CommentsDialog.ui MillingMachineProgramDialog.ui LogDialog.ui ChangeModulateurDialog.ui ChangeDegradeurDialog.ui AddSeanceDialog.ui ModifyPatientNameDialog.ui \
        ModifyDeliveredDoseDialog.ui CreateDossierDialog.ui ShowDeliveredDoseDialog.ui DeleteSeanceDialog.ui AssociateDialog.ui ModifyPlannedDoseDialog.ui \
    PrintDialog.ui \
        SelectPatientDialog.ui

win32:CONFIG(release, debug|release): LIBS += \
-L$$OUT_PWD/../MillingMachine/src/release/ -lMillingMachine \
-L$$OUT_PWD/../DataStructures/protontherapy/src/release/ -lPTDataStructures \
-L$$OUT_PWD/../DataStructures/qualityassurance/src/release/ -lQADataStructures \
-L$$OUT_PWD/../Repository/protontherapy/src/release/ -lPTRepo \
-L$$OUT_PWD/../Repository/qualityassurance/src/release/ -lQARepo \
-L$$OUT_PWD/../External/qcustomplot/release/ -lqcustomplot \
-L$$OUT_PWD/../Util/src/release/ -lUtil \
-L$$OUT_PWD/../Graphs/src/release/ -lGraphs \
-L$$OUT_PWD/../Printing/release/ -lPrinting
else:win32:CONFIG(debug, debug|release): LIBS += \
-L$$OUT_PWD/../MillingMachine/src/debug/ -lMillingMachine \
-L$$OUT_PWD/../DataStructures/protontherapy/src/debug/ -lPTDataStructures \
-L$$OUT_PWD/../DataStructures/qualityassurance/src/debug/ -lQADataStructures \
-L$$OUT_PWD/../Repository/protontherapy/src/debug/ -lPTRepo \
-L$$OUT_PWD/../Repository/qualityassurance/src/debug/ -lQARepo \
-L$$OUT_PWD/../Util/src/debug/ -lUtil \
-L$$OUT_PWD/../External/qcustomplot/debug/ -lqcustomplot \
-L$$OUT_PWD/../Graphs/src/debug/ -lGraphs \
-L$$OUT_PWD/../Printing/debug/ -lPrinting
else:unix: LIBS += \
-L$$OUT_PWD/../MillingMachine/src/ -lMillingMachine \
-L$$OUT_PWD/../DataStructures/protontherapy/src/ -lPTDataStructures \
-L$$OUT_PWD/..//DataStructures/qualityassurance/src/ -lQADataStructures \
-L$$OUT_PWD/../Repository/protontherapy/src/ -lPTRepo \
-L$$OUT_PWD/../Repository/qualityassurance/src/ -lQARepo \
-L$$OUT_PWD/../Util/src/ -lUtil \
-L$$OUT_PWD/../External/qcustomplot/ -lqcustomplot \
-L$$OUT_PWD/../Graphs/src/ -lGraphs \
-L$$OUT_PWD/../Printing/ -lPrinting


INCLUDEPATH += \
$$PWD/../MillingMachine/src \
$$PWD/../DataStructures/protontherapy/src \
$$PWD/../DataStructures/qualityassurance/src \
$$PWD/../Repository/protontherapy/src \
$$PWD/../Repository/qualityassurance/src \
$$PWD/../External/qcustomplot \
$$PWD/../Util/src \
$$PWD/../Graphs/src \
$$PWD/../Printing
DEPENDPATH += \
$$PWD/../MillingMachine/src \
$$PWD/../DataStructures/protontherapy/src \
$$PWD/../DataStructures/qualityassurance/src \
$$PWD/../Repository/protontherapy/src \
$$PWD/../Repository/qualityassurance/src \
$$PWD/../Util/src \
$$PWD/../External/qcustomplot \
$$PWD/../Graphs/src \
$$PWD/../Printing

win32: RESOURCES += ../Styles/breeze_win32.qrc
else:unix: RESOURCES += ../Styles/breeze_unix.qrc

RESOURCES += resources.qrc
