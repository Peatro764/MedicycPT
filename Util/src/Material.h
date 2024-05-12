#ifndef QUALITYASSURANCE_MATERIAL_H
#define QUALITYASSURANCE_MATERIAL_H

#include <QString>

namespace material {

const double PLEXI2TISSUE = 1.106;
const double WATER2TISSUE = 0.961739*0.9913794;
enum class MATERIAL : int { WATER = 0, TISSUE = 1, PLEXIGLAS = 2, AIR = 3 };

// See IAEA TRS398
double ToPlexiglas(double length, MATERIAL material);
double ToTissue(double length, MATERIAL material);
double ToWater(double length, MATERIAL material);

// KEEP FOR THE MOMENT, BUT EVENTUALLY EVERYTHING SHOULD USE ABOVE FUNCTIONS (for dose rate calculation, Joel approval before switching)
double Tissue2Plexiglas(const double& l_tissue);
double Plexiglas2Tissue(const double& l_plexiglas);

}

#endif
