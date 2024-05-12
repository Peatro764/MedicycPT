#ifndef COMPENSATEUR_H
#define COMPENSATEUR_H

#include <QSettings>
#include <QString>
#include <vector>
#include <QPolygon>
#include <QVector>

#include "Eye.h"
#include "qcustomplot.h"

class Compensateur
{

public:
    Compensateur(const int& dossier, const Eye& eye, const QCPCurveData& center_point, const double& bolus_thickness,
                 const double& parcours, const double& parcours_error, const double& error_scaling, const double& base_thickness);
    Compensateur();

    QCPCurveDataContainer FraiseuseFormat() const;
    QCPCurveDataContainer FullStepCurve() const;
    QCPCurveDataContainer FullTheoCurve() const;
    QCPCurveDataContainer HalfStepCurve() const;
    QCPCurveDataContainer HalfTheoCurve() const;

    bool IsValid() const { return is_valid_; }
    int dossier() const { return dossier_; }
    double mm_plexiglas(const double& radius) const;
    double bolus_thickness() const { return bolus_thickness_; }
    double parcours() const { return parcours_; }
    double parcours_error() const { return parcours_error_; }
    double error_scaling() const { return error_scaling_; }
    QCPCurveData center_point() const { return center_point_; }
    double base_thickness() const { return base_thickness_; }
    Eye eye() const { return eye_; }

    static QString program_type;

private:
    double GetPlexiglasColumnHeight() const;
    QCPCurveDataContainer MirrorOverValueAxis(const QCPCurveDataContainer& curve) const;

    int dossier_;
    Eye eye_;
    QCPCurveData center_point_;
    double bolus_thickness_;
    double parcours_;
    double parcours_error_;
    double error_scaling_;
    double base_thickness_;
    bool is_valid_;

    QCPCurveDataContainer half_step_curve_;
    QCPCurveDataContainer half_theo_curve_;
};

bool operator==(const Compensateur &c1, const Compensateur& c2);
bool operator!=(const Compensateur &c1, const Compensateur& c2);


#endif // COMPENSATEUR_H
