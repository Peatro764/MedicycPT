QT += core \
      network \
      sql
TARGET = DataAcquisition
TEMPLATE = lib

DEFINES += DATAACQUISITION_LIBRARY
SOURCES += DevFactory.cpp MockMotor.cpp StepMotor.cpp ServoMotor.cpp Sensor.cpp PositionSensor.cpp Clock.cpp Actuator.cpp LabJack.cpp
HEADERS += DevFactory.h IMotor.h MockMotor.h StepMotor.h ServoMotor.h ISensor.h Sensor.h PositionSensor.h Actuator.h Clock.h LabJack.h

win32:CONFIG(release, debug|release): LIBS += \
-L$$OUT_PWD/../../../DataStructures/qualityassurance/release/ -lQADataStructures \
-L$$OUT_PWD/../../../External/qcustomplot/release/ -lqcustomplot \
-L$$OUT_PWD/../../../Repository/qualityassurance/release/ -lQARepo
else:win32:CONFIG(debug, debug|release): LIBS += \
-L$$OUT_PWD/../../../DataStructures/qualityassurance/src/debug/ -lQADataStructures \
-L$$OUT_PWD/../../../External/qcustomplot/debug/ -lqcustomplot \
-L$$OUT_PWD/../../../Repository/qualityassurance/src/debug/ -lQARepo
else:unix: LIBS += \
-L$$OUT_PWD/../../../DataStructures/qualityassurance/src/ -lQADataStructures \
-L$$OUT_PWD/../../../External/qcustomplot/ -lqcustomplot \
-L$$OUT_PWD/../../../Repository/qualityassurance/src/ -lQARepo \
-L$$OUT_PWD/../../../Util/src/ -lUtil

INCLUDEPATH += \
$$PWD/../../../DataStructures/qualityassurance/src \
$$PWD/../../../External/qcustomplot \
$$PWD/../../../Repository/qualityassurance/src \
$$PWD/../../../Util/src \

DEPENDPATH += \
$$PWD/../../../DataStructures/qualityassurance/src \
$$PWD/../../../External/qcustomplot \
$$PWD/../../../Repository/qualityassurance/src \
$$PWD/../../../Util/src \


#ext_libs.path = $$OUT_PWD
#ext_libs.files = LabJackM.h LJM_Utilities.h libLabJackM.so config
#INSTALLS = ext_libs

#RESOURCES = labjack_config.qrc
