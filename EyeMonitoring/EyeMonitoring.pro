TEMPLATE = app
TARGET = EyeMonitoring
QMAKE_CXXFLAGS += -std=c++14
QT += core \
    gui \
    sql \
    xml \
    network \
    opengl \
    widgets \
    charts \
    printsupport \
    dbus
HEADERS = EyeMonitoring.h DrawingArea.h NumericInputDialog.h RecordingWidget.h \
    MRMThread.h VideoWorker.h DBWorker.h
SOURCES = EyeMonitoring.cpp DrawingArea.cpp NumericInputDialog.cpp RecordingWidget.cpp \
    MRMThread.cpp VideoWorker.cpp DBWorker.cpp \
    main.cpp

DBUS_INTERFACES += ../DBus/RadiationMonitor.xml

FORMS = EyeMonitoring.ui NumericInputDialog.ui
win32: RESOURCES += ../Styles/breeze_win32.qrc
else:unix: RESOURCES += ../Styles/breeze_unix.qrc
RESOURCES += resources.qrc

LIBS += \
-lopencv_core -lopencv_videoio \
-L$$OUT_PWD/../Util/src/ -lUtil \
-L$$OUT_PWD/../Repository/protontherapy/src/ -lPTRepo \
-L$$OUT_PWD/../External/qcustomplot/ -lqcustomplot \
-L$$OUT_PWD/../DataStructures/protontherapy/src/ -lPTDataStructures

INCLUDEPATH += \
/usr/include/opencv4/ \
$$PWD/../Util/src \
$$PWD/../External/qcustomplot \
$$PWD/../Repository/protontherapy/src \
$$PWD/../DataStructures/protontherapy/src

DEPENDPATH += \
$$PWD/../Util/src \
$$PWD/../External/qcustomplot \
$$PWD/../Repository/protontherapy/src \
$$PWD/../DataStructures/protontherapy/src
