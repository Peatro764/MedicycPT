#ifndef MODULATEURMAKER_H
#define MODULATEURMAKER_H

#include <map>

#include "DepthDoseCurve.h"
#include "MCNPXData.h"

typedef std::map<int, double> um_plexi_weight;

class ModulateurMaker
{
public:
    ModulateurMaker();
    ModulateurMaker(const um_plexi_depthDoseCurve& depth_dose_curves);

    static std::vector<int> GetRequiredUmPlexiSteps(double parc_mm_tissue, double mod_mm_tissue,
                                                    double max_parc_mm_tissue, double decalage_mm_tissue,
                                                    double mod_step_size_mm_plexi);
    static um_plexi_depthDoseCurve InterpolateDepthDoseCurves(const um_plexi_depthDoseCurve& raw_curves, const std::vector<int>& um_plexi_vector);

    um_plexi_weight GetInitialWeights() const;
    um_plexi_weight IterateWeights(const um_plexi_weight& weights);
    um_plexi_depthDoseCurve GetWeightedCurves(const um_plexi_weight& weights) const;
    DepthDoseCurve GetSumOfWeightedCurves(const um_plexi_weight& weights) const;
    bool HasConverged(const um_plexi_weight& new_weights, const um_plexi_weight& old_weights, double eps_convergence) const;

private:
    double SumCurvesAtPosition(const um_plexi_depthDoseCurve& curves, double position) const;
    double GetCorrectionFactor(int peak_number) const;
    double CalculateCurveWeight(double sum_curves_at_primary_peak, double sum_curves_at_current_peak,
                                double max_current_peak, double correction_factor);

    um_plexi_depthDoseCurve depth_dose_curves_;
};


#endif // MODULATEURMAKER_H
