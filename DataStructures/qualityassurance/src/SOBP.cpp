#include "SOBP.h"

#include "Calc.h"

SOBP::SOBP(DepthDoseMeasurement depth_dose,
           int dossier, int modulateur_id, double mm_degradeur, QString comment, double penumbra, double parcours, double mod_98, double mod_100)
    : depth_dose_(depth_dose), dossier_(dossier), modulateur_id_(modulateur_id), mm_degradeur_(mm_degradeur), comment_(comment), penumbra_(penumbra), parcours_(parcours),
      mod_98_(mod_98), mod_100_(mod_100) {}


bool operator==(const SOBP& s1, const SOBP& s2) {
    return (s1.depth_dose() == s2.depth_dose() &&
            s1.modulateur_id() == s2.modulateur_id() &&
            s1.dossier() == s2.dossier() &&
            calc::AlmostEqual(s1.mm_degradeur(), s2.mm_degradeur(), 0.0001) &&
            calc::AlmostEqual(s1.parcours(), s2.parcours(), 0.0001) &&
            calc::AlmostEqual(s1.penumbra(), s2.penumbra(), 0.0001) &&
            calc::AlmostEqual(s1.mod98(), s2.mod98(), 0.0001) &&
            calc::AlmostEqual(s1.mod100(), s2.mod100(), 0.0001));
}

bool operator!=(const SOBP& p1, const SOBP& p2) {
    return !(p1 == p2);
}
