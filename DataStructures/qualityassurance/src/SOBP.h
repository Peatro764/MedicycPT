#ifndef QUALITYASSURANCE_SOBP_H
#define QUALITYASSURANCE_SOBP_H

#include <vector>
#include <QPointF>
#include <QDateTime>

#include "DepthDoseMeasurement.h"

class SOBP
{
public:
    SOBP(DepthDoseMeasurement depth_dose,
         int dossier, int modulateur_id, double mm_degradeur, QString comment,
         double penumbra, double parcours, double mod_98, double mod_100);

    DepthDoseMeasurement depth_dose() const { return depth_dose_; }
    int dossier() const { return dossier_; }
    int modulateur_id() const { return modulateur_id_; }
    double mm_degradeur() const { return mm_degradeur_; }
    QString comment() const {  return comment_; }

    double penumbra() const { return penumbra_; }
    double parcours() const { return parcours_; }
    double mod98() const { return mod_98_; }
    double mod100() const { return mod_100_; }

private:
    DepthDoseMeasurement depth_dose_;
    int dossier_;
    int modulateur_id_;
    double mm_degradeur_;
    QString comment_;
    double penumbra_;
    double parcours_;
    double mod_98_;
    double mod_100_;
};

bool operator==(const SOBP& b1, const SOBP& b2);
bool operator!=(const SOBP& b1, const SOBP& b2);


#endif
