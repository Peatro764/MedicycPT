#ifndef QUALITYASSURANCE_HARDWARE_H
#define QUALITYASSURANCE_HARDWARE_H

#include <QString>

#include "Material.h"

enum class QA_HARDWARE : int { SCANNER2D = 0, SCANNER3D = 1, WHEEL = 2, UNK = 3 };
QString HardwareToString(QA_HARDWARE hardware);
QA_HARDWARE StringToHardware(QString hardware);
material::MATERIAL IsMadeOf(QA_HARDWARE hardware);

#endif
