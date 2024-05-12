#include "Axis.h"

QString AxisToString(Axis axis) {
    QString name("unknown");
    switch(axis) {
    case Axis::X:
        name = "X";
        break;
    case Axis::Y:
        name = "Y";
        break;
    case Axis::Z:
        name = "Z";
        break;
    default:
        name = "unknown";
        break;
    }
    return name;
}

Axis StringToAxis(QString axis) {
    if (axis == QString("X")) {
        return Axis::X;
    } else if (axis == QString("Y")) {
        return Axis::Y;
    } else if (axis == QString("Z")) {
        return Axis::Z;
    } else {
        return Axis::UNK;
    }
}

