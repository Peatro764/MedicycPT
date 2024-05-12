#ifndef QUALITYASSURANCE_AXIS_H
#define QUALITYASSURANCE_AXIS_H

#include <QString>

enum class Axis : int { X = 0, Y = 1, Z = 2, UNK = 3 };
QString AxisToString(Axis axis);
Axis StringToAxis(QString axis);

#endif
