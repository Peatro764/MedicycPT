#include "Hardware.h"

#include <stdexcept>

QString HardwareToString(QA_HARDWARE hardware) {
    QString name("unknown");
    switch(hardware) {
    case QA_HARDWARE::SCANNER2D:
        name = "SCANNER2D";
        break;
    case QA_HARDWARE::SCANNER3D:
        name = "SCANNER3D";
        break;
    case QA_HARDWARE::WHEEL:
        name = "WHEEL";
        break;
    default:
        name = "unknown";
        break;
    }
    return name;
}

QA_HARDWARE StringToHardware(QString hardware) {
    if (hardware == QString("SCANNER2D")) {
        return QA_HARDWARE::SCANNER2D;
    } else if (hardware == QString("SCANNER3D")) {
        return QA_HARDWARE::SCANNER3D;
    } else if (hardware == QString("WHEEL")) {
        return QA_HARDWARE::WHEEL;
    } else {
        return QA_HARDWARE::UNK;
    }
}

material::MATERIAL IsMadeOf(QA_HARDWARE hardware) {
    switch(hardware) {
    case QA_HARDWARE::SCANNER2D:
        return material::MATERIAL::AIR;
        break;
    case QA_HARDWARE::SCANNER3D:
        return material::MATERIAL::WATER;
        break;
    case QA_HARDWARE::WHEEL:
        return material::MATERIAL::PLEXIGLAS;
        break;
    default:
        throw std::runtime_error("Unknown hardware choice");
        break;
    }
}
