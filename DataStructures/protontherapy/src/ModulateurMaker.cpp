#include "ModulateurMaker.h"

#include <cmath>

#include "Calc.h"
#include "Util.h"
#include "Material.h"

ModulateurMaker::ModulateurMaker() {}

ModulateurMaker::ModulateurMaker(const um_plexi_depthDoseCurve& depth_dose_curves) :
    depth_dose_curves_(depth_dose_curves)
{
    if (depth_dose_curves.empty()) {
        throw std::runtime_error("Depth dose curves are empty");
    }
}

std::vector<int> ModulateurMaker::GetRequiredUmPlexiSteps(double parc_mm_tissue, double mod_mm_tissue,
                                                          double max_parc_mm_tissue, double decalage_mm_plexi,
                                                          double mod_step_size_mm_plexi) {
    if (parc_mm_tissue < 0.0 || parc_mm_tissue > max_parc_mm_tissue) {
        throw std::runtime_error("Parcours parameter(s) out of bounds");
    }
    if (mod_mm_tissue <= 0.0 || mod_mm_tissue > max_parc_mm_tissue) {
        throw std::runtime_error("Modulateur parameter out of bounds");
    }
    if (decalage_mm_plexi < 0.0 || decalage_mm_plexi > max_parc_mm_tissue) {
        throw std::runtime_error("Decalage parameter out of bounds");
    }

    const double deg_mm_plexi(material::Tissue2Plexiglas(max_parc_mm_tissue - parc_mm_tissue) - decalage_mm_plexi);
    int min_um_plexi(static_cast<int>(1000.0 * deg_mm_plexi));
    min_um_plexi -= min_um_plexi % 100; // to have one decimal in mm
    const int n_steps(1 + static_cast<int>(std::round(material::Tissue2Plexiglas(mod_mm_tissue) / mod_step_size_mm_plexi)));
    std::vector<int> um_steps;
    for (int step = 0; step < n_steps; ++step) {
        um_steps.push_back(min_um_plexi + step * static_cast<int>(mod_step_size_mm_plexi * 1000.0));
    }
    return um_steps;
}

um_plexi_weight ModulateurMaker::GetInitialWeights() const {
    um_plexi_weight weights;
    for (auto& plexi_curve : depth_dose_curves_) {
        weights.emplace(plexi_curve.first, 1.0);
    }
    return weights;
}

um_plexi_weight ModulateurMaker::IterateWeights(const um_plexi_weight &weights) {
    if (weights.size() != depth_dose_curves_.size()) {
        throw std::runtime_error("Depthdosecurves and weights have different number of thickness steps");
    }
    auto weighted_curves(GetWeightedCurves(weights));
    DepthDose primary_peak_max(weighted_curves.begin()->second.Max());
    um_plexi_weight new_weights;

    new_weights.emplace(weights.begin()->first, weights.begin()->second); // weight of primary peak always 1.0
    for (auto it = (++weights.begin()); it != weights.end(); ++it) { // dont recalculate weight of primary peak, i.e start at ++weights.begin()
        double sum_curves_at_primary_peak(SumCurvesAtPosition(weighted_curves, primary_peak_max.depth()));
        DepthDose current_curve_max(weighted_curves.at(it->first).Max());
        double sum_curves_at_current_peak(SumCurvesAtPosition(weighted_curves, current_curve_max.depth()));
        const double weight(std::max(0.0, CalculateCurveWeight(sum_curves_at_primary_peak, sum_curves_at_current_peak,
                                                               current_curve_max.dose(), GetCorrectionFactor(std::distance(weights.begin(), it)))));
        weighted_curves.at(it->first) = weight * weighted_curves.at(it->first);
        new_weights.emplace(it->first, weight * it->second);
    }
    return new_weights;
}

um_plexi_depthDoseCurve ModulateurMaker::InterpolateDepthDoseCurves(const um_plexi_depthDoseCurve& raw_curves, const std::vector<int>& um_plexi_vector) {
    um_plexi_depthDoseCurve interpolated_curves;
    for (auto um_plexi : um_plexi_vector) {
        if (um_plexi <= raw_curves.rbegin()->first) {
            interpolated_curves.emplace(um_plexi, util::InterpolateDepthDoseCurve(um_plexi, raw_curves));
        } else {
            interpolated_curves.emplace(um_plexi, raw_curves.rbegin()->second);
        }
    }
    return interpolated_curves;
}

bool ModulateurMaker::HasConverged(const um_plexi_weight &new_weights, const um_plexi_weight &old_weights,
                                   double eps_convergence) const {
    for (auto& w : new_weights) {
        if (std::abs(w.second - old_weights.at(w.first)) > eps_convergence || std::isnan(w.second)) {
            return false;
        }
    }
    return true;
}

um_plexi_depthDoseCurve ModulateurMaker::GetWeightedCurves(const um_plexi_weight& weights) const {
    um_plexi_depthDoseCurve weighted_curves;
    for (auto& um_plexi_weight : weights) {
        weighted_curves[um_plexi_weight.first] = um_plexi_weight.second * depth_dose_curves_.at(um_plexi_weight.first);
    }
    return weighted_curves;
}

DepthDoseCurve ModulateurMaker::GetSumOfWeightedCurves(const um_plexi_weight& weights) const {
    um_plexi_depthDoseCurve weighted_curves(GetWeightedCurves(weights));
    DepthDoseCurve sum_curves;
    for (auto& el : weighted_curves) {
        sum_curves = sum_curves + el.second;
    }
    return sum_curves;
}


double ModulateurMaker::CalculateCurveWeight(double sum_curves_at_primary_peak, double sum_curves_at_current_peak,
                                             double max_current_peak, double correction_factor) {
    return 1.0 + (sum_curves_at_primary_peak * correction_factor - sum_curves_at_current_peak) / max_current_peak;

}

double ModulateurMaker::SumCurvesAtPosition(const um_plexi_depthDoseCurve& curves, double position) const {
    double sum(0.0);
    for (auto &curve : curves) {
        sum += curve.second.Dose(position).dose();
    }
    return sum;
}

double ModulateurMaker::GetCorrectionFactor(int peak_number) const {
    switch (peak_number) {
    case 0:
        return 1.0;
        break;
    case 1:
        return 1.09;
        break;
    case 2:
        return 1.11;
        break;
    case 3:
        return 1.13;
        break;
     case 4:
        return 1.13;
        break;
     default:
        return 1.14;
        break;
    }

    throw std::runtime_error("Error retrieving the correction factor");
    return 0.0; // to remove compiler warning
}
