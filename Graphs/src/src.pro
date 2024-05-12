TEMPLATE = lib
TARGET = Graphs
QMAKE_CXXFLAGS += -std=c++11
QT += core \
    gui \
    widgets \
    charts \
    printsupport
HEADERS += TimeSeries.h TimedStampedDataGraph.h DoseConsistencyGraph.h ModulateurGraph.h ModulateurDialog.h DebitGraph.h TimeSeriesGraph.h LinePlot.h LinePlotMeasurementDialog.h Histogram.h CollimatorWidget.h CollimateurDialog.h CompensateurTransparantDialog.h SeanceDoseGraph.h LoginDialog.h SeanceProgressGraph.h \
    PasswordConfirmation.h
SOURCES += TimeSeries.cpp TimedStampedDataGraph.cpp DoseConsistencyGraph.cpp ModulateurGraph.cpp ModulateurDialog.cpp DebitGraph.cpp TimeSeriesGraph.cpp LinePlot.cpp LinePlotMeasurementDialog.cpp Histogram.cpp CollimatorWidget.cpp CollimateurDialog.cpp CompensateurTransparantDialog.cpp SeanceDoseGraph.cpp LoginDialog.cpp SeanceProgressGraph.cpp \
    PasswordConfirmation.cpp

FORMS = ModulateurDialog.ui CompensateurTransparantDialog.ui CollimateurDialog.ui LinePlotMeasurementDialog.ui

RESOURCES +=
    
DEFINES += GRAPHS_LIBRARY

win32:CONFIG(release, debug|release): LIBS += \
-L$$OUT_PWD/../../External/qcustomplot/release/ -lqcustomplot \
-L$$OUT_PWD/../../DataStructures/protontherapy/src/release/ -lPTDatastructures \
-L$$OUT_PWD/../../DataStructures/qualityassurance/src/release/ -lQADatastructures \
-L$$OUT_PWD/../../Util/src/release/ -lUtil
else:win32:CONFIG(debug, debug|release): LIBS += \
-L$$OUT_PWD/../../External/qcustomplot/debug/ -lqcustomplot \
-L$$OUT_PWD/../../DataStructures/protontherapy/src/debug/ -lPTDatastructures \
-L$$OUT_PWD/../../DataStructures/qualityassurance/src/debug/ -lQADatastructures \
-L$$OUT_PWD/../../Util/src/debug/ -lUtil
else:unix: LIBS += \
-L$$OUT_PWD/../../External/qcustomplot/ -lqcustomplot \
-L$$OUT_PWD/../../DataStructures/protontherapy/src/ -lPTDataStructures \
-L$$OUT_PWD/../../DataStructures/qualityassurance/src/ -lQADataStructures \
-L$$OUT_PWD/../../Util/src/ -lUtil

INCLUDEPATH += \
$$PWD/../../External/qcustomplot \
$$PWD/../../DataStructures/protontherapy/src/ \
$$PWD/../../DataStructures/qualityassurance/src/ \
$$PWD/../../Util/src/
DEPENDPATH += \
$$PWD/../../External/qcustomplot \
$$PWD/../../DataStructures/protontherapy/src/ \
$$PWD/../../DataStructures/qualityassurance/src/ \
$$PWD/../../Util/src
