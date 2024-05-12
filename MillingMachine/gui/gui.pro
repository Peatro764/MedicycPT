TEMPLATE = app
TARGET = MillingMachineGui
QMAKE_CXXFLAGS += -std=c++11
QT += core \
    gui \
    sql \
    network \
    widgets \
    serialport \
    printsupport
HEADERS +=  MillingMachineGui.h MillingMachineIODialog.h ProgramTypeDialog.h ConfigDialog.h FetchProgramDialog.h SaveProgramDialog.h
SOURCES += MillingMachineGui.cpp MillingMachineIODialog.cpp ProgramTypeDialog.cpp ConfigDialog.cpp FetchProgramDialog.cpp SaveProgramDialog.cpp main.cpp
FORMS += MillingMachineGui.ui ProgramTypeDialog.ui FetchProgramDialog.ui SaveProgramDialog.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../src/release/ -lMillingMachine
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../src/debug/ -lMillingMachine
else:unix: LIBS += -L$$OUT_PWD/../src/ -lMillingMachine

INCLUDEPATH += $$PWD/../src
DEPENDPATH += $$PWD/../src

win32:CONFIG(release, debug|release): LIBS += \
-L$$OUT_PWD/../../Repository/protontherapy/src/release/ -lPTRepo \
-L$$OUT_PWD/../../DataStructures/protontherapy/src/release/ -lPTDataStructures \
-L$$OUT_PWD/../../External/qcustomplot/release/ -lqcustomplot \
-L$$OUT_PWD/../../Util/src/release/ -lUtil
else:win32:CONFIG(debug, debug|release): LIBS += \
-L$$OUT_PWD/../../Repository/protontherapy/src/debug/ -lPTRepo \
-L$$OUT_PWD/../../DataStructures/protontherapy/src/debug/ -lPTDataStructures \
-L$$OUT_PWD/../../External/qcustomplot/debug/ -lqcustomplot \
-L$$OUT_PWD/../../Util/src/debug/ -lUtil
else:unix: LIBS += \
-L$$OUT_PWD/../../Repository/protontherapy/src/ -lPTRepo \
-L$$OUT_PWD/../../Util/src/ -lUtil \
-L$$OUT_PWD/../../External/qcustomplot/ -lqcustomplot \
-L$$OUT_PWD/../../DataStructures/protontherapy/src/ -lPTDataStructures

INCLUDEPATH += \
$$PWD/../../Repository/protontherapy/src \
$$PWD/../../DataStructures/protontherapy/src \
$$PWD/../../Util/src \
$$PWD/../../External/qcustomplot
DEPENDPATH += \
$$PWD/../../Repository/protontherapy/src \
$$PWD/../../DataStructures/protontherapy/src \
$$PWD/../../Util/src \
$$PWD/../../External/qcustomplot

win32: RESOURCES += ../../Styles/breeze_win32.qrc
else:unix: RESOURCES += ../../Styles/breeze_unix.qrc

RESOURCES += resources.qrc
