#include "Compensateur.h"

#include <QDebug>

#include "Calc.h"
#include "Material.h"
#include "QCPCurveUtils.h"

QString Compensateur::program_type = "COMPENSATEUR";

Compensateur::Compensateur() :
    is_valid_(false) {}

Compensateur::Compensateur(const int& dossier, const Eye& eye, const QCPCurveData& center_point, const double& bolus_thickness,
                           const double& parcours, const double& parcours_error, const double& error_scaling, const double& base_thickness) :
    dossier_(dossier),
    eye_(eye),
    center_point_(center_point),
    bolus_thickness_(bolus_thickness),
    parcours_(parcours),
    parcours_error_(parcours_error),
    error_scaling_(error_scaling),
    base_thickness_(base_thickness),
    is_valid_(true) {

    if (parcours_ < (eye.sclere_thickness()  + bolus_thickness_)) {
        throw std::runtime_error("Parcours must be >= (sclere + bolus)");
    }

    // get sclere lower curvature and correct shape for
    // air / plexiglas proton response difference
    auto sclere_upper_right(eye_.SclereCurve(M_PI / 2.0, 0.0));
    qcpcurveutils::ScaleValueAxis(sclere_upper_right, material::Tissue2Plexiglas(1.0));

    // add plexiglas column to get correct parcours
    qcpcurveutils::SetMaxValue(sclere_upper_right, GetPlexiglasColumnHeight());

    // cut curve at 0 (the curve will go below 0 when a parcours not large enough to create
    // a compensateur extending to the horizon was given.
    QCPCurveDataContainer sclere_upper_right_cutted(qcpcurveutils::CutAt(sclere_upper_right, 0.0, true));

    // add zero point
    sclere_upper_right_cutted.add(QCPCurveData(sclere_upper_right_cutted.at(sclere_upper_right_cutted.size() - 1)->t + 0.1,
                                        sclere_upper_right_cutted.at(sclere_upper_right_cutted.size() - 1)->key,
                                        0.0));

    // discretize curve and convert to step function
    QCPCurveDataContainer discretized(qcpcurveutils::Discretize(sclere_upper_right_cutted, material::Tissue2Plexiglas(parcours_error_), error_scaling));

    half_step_curve_ = qcpcurveutils::StepCurve(discretized, 0.0);
    half_theo_curve_ = sclere_upper_right_cutted;
}

QCPCurveDataContainer Compensateur::FraiseuseFormat() const {
    QCPCurveDataContainer fraiseuse_format;
    for (int idx = 0; idx < half_step_curve_.size(); ++idx) {
        if (!(idx % 2)) {
            fraiseuse_format.add(*half_step_curve_.at(idx));
        }
    }
    return fraiseuse_format;
}

QCPCurveDataContainer Compensateur::MirrorOverValueAxis(const QCPCurveDataContainer& curve) const {
    QCPCurveDataContainer mirrored;
    int t(0);
    for (int idx = (curve.size() - 1); idx >= 0; --idx) {
        mirrored.add(QCPCurveData(t++, -curve.at(idx)->key, curve.at(idx)->value));
    }
    for (int idx = 0; idx < curve.size(); ++idx) {
        mirrored.add(QCPCurveData(t++, curve.at(idx)->key, curve.at(idx)->value));
    }
    return mirrored;
}

QCPCurveDataContainer Compensateur::FullStepCurve() const {
    return MirrorOverValueAxis(half_step_curve_);
}

QCPCurveDataContainer Compensateur::FullTheoCurve() const {
    return MirrorOverValueAxis(half_theo_curve_);
}

QCPCurveDataContainer Compensateur::HalfStepCurve() const {
    return half_step_curve_;
}

QCPCurveDataContainer Compensateur::HalfTheoCurve() const {
    return half_theo_curve_;
}

double Compensateur::mm_plexiglas(const double& radius) const {
    for (int idx = 0; idx < half_step_curve_.size(); ++idx) {
        if (half_step_curve_.at(idx)->mainKey() >= radius) {
            return half_step_curve_.at(idx)->value;
        }
    }
    return 0.0;
}

double Compensateur::GetPlexiglasColumnHeight() const {
    const double height_tissue(parcours_ - bolus_thickness_ - eye_.sclere_thickness());
    return material::Tissue2Plexiglas(height_tissue);
}

bool operator==(const Compensateur &c1, const Compensateur& c2) {
    return (c1.dossier() == c2.dossier() &&
            c1.eye() == c2.eye() &&
            (c1.center_point() == c2.center_point()) &&
            calc::AlmostEqual(c1.bolus_thickness(), c2.bolus_thickness(), 0.001) &&
            calc::AlmostEqual(c1.parcours(), c2.parcours(), 0.001) &&
            calc::AlmostEqual(c1.parcours_error(), c2.parcours_error(), 0.001) &&
            calc::AlmostEqual(c1.error_scaling(), c2.error_scaling(), 0.001) &&
            calc::AlmostEqual(c1.base_thickness(), c2.base_thickness(), 0.001));
}

bool operator!=(const Compensateur &c1, const Compensateur& c2) {
    return !(c1 == c2);
}
