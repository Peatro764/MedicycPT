#include "DepthDoseCurve.h"

#include <algorithm>
#include <stdexcept>
#include <iterator>
#include <math.h>

#include "Calc.h"

DepthDoseCurve::DepthDoseCurve() {}

DepthDoseCurve::DepthDoseCurve(const std::vector<DepthDose>& depth_doses) :
    depth_doses_(depth_doses)
{
    Sort();
}

void DepthDoseCurve::Sort() {
    auto SortIncDepth = [&] (const DepthDose& d1, const DepthDose& d2) -> bool { return (d1.depth() < d2.depth()); };
    std::sort(depth_doses_.begin(), depth_doses_.end(), SortIncDepth);
}

bool DepthDoseCurve::InInterval(double depth) const {
    return (depth >= depth_doses_.begin()->depth() && depth <= depth_doses_.rbegin()->depth());
}


DepthDose DepthDoseCurve::Dose(double depth) const {
    auto InDepthInterval = [&] (const DepthDose& d1, const DepthDose& d2) -> bool { return (depth >= d1.depth() && depth <= d2.depth()); };
    auto it = std::adjacent_find(depth_doses_.begin(), depth_doses_.end(), InDepthInterval);
    if (it == depth_doses_.end()) {
        throw std::runtime_error("Depth not in DepthDoseInterval");
    }
    const DepthDose d1 = *it;
    const DepthDose d2 = *(++it);
    const double interpolated_dose(d1.dose() + ((d2.dose() - d1.dose()) * ((depth - d1.depth()) / (d2.depth() - d1.depth()))));
    return DepthDose(depth, interpolated_dose);
}

DepthDose DepthDoseCurve::Max() const {
    if (depth_doses_.empty()) {
        throw std::runtime_error("Trying to take max value from empty DepthDoseCurve");
    }
    auto comp = [&] (const DepthDose& d1, const DepthDose& d2) -> bool { return d1.dose() < d2.dose(); };
    return *std::max_element(depth_doses_.begin(), depth_doses_.end(), comp);
}

DepthDoseCurve DepthDoseCurve::CapAtZero() const {
    std::vector<DepthDose> capped_depth_doses;
    for (auto depth_dose : depth_doses_) {
        capped_depth_doses.push_back(DepthDose(depth_dose.depth(), std::abs(depth_dose.dose())));
    }
    return DepthDoseCurve(capped_depth_doses);
}

DepthDoseCurve operator+(const DepthDoseCurve& c1, const DepthDoseCurve& c2) {
    if ((c1.depth_doses_.size() != c2.depth_doses_.size()) && (!c1.depth_doses_.empty() && !c2.depth_doses_.empty())) {
        throw std::runtime_error("Cannot add two depth dose curves with different size");
    }

    if (c1.depth_doses_.empty()) {
        return c2;
    }

    if (c2.depth_doses_.empty()) {
        return c1;
    }

    std::vector<DepthDose> summed_depth_doses;
    for (int idx = 0; idx < (int)c1.depth_doses_.size(); ++idx) {
        summed_depth_doses.push_back(c1.depth_doses_.at(idx) + c2.depth_doses_.at(idx));
    }
    return DepthDoseCurve(summed_depth_doses);
}

DepthDoseCurve operator-(const DepthDoseCurve& c) {
    std::vector<DepthDose> negated_depth_doses;
    for (auto depth_dose : c.depth_doses_) {
        negated_depth_doses.push_back(-depth_dose);
    }
    return negated_depth_doses;
}

DepthDoseCurve operator-(const DepthDoseCurve& c1, const DepthDoseCurve& c2) {
    if ((c1.depth_doses_.size() != c2.depth_doses_.size()) && (!c1.depth_doses_.empty() && !c2.depth_doses_.empty())) {
        throw std::runtime_error("Cannot add two depth dose curves with different size");
    }

    if (c1.depth_doses_.empty()) {
        return -c2;
    }

    if (c2.depth_doses_.empty()) {
        return -c1;
    }

    std::vector<DepthDose> subtracted_depth_doses;
    for (int idx = 0; idx < (int)c1.depth_doses_.size(); ++idx) {
        subtracted_depth_doses.push_back(c1.depth_doses_.at(idx) - c2.depth_doses_.at(idx));
    }
    return DepthDoseCurve(subtracted_depth_doses);
}

DepthDoseCurve operator*(const double& p, const DepthDoseCurve& c) {
    if (p < 0.0) {
        throw std::runtime_error("Cannot multiply a DepthDoseCurve with a negative operator");
    }

    if (c.depth_doses_.empty()) {
        return DepthDoseCurve();
    }

    std::vector<DepthDose> multiplied_depth_doses;
    for (auto depth_dose : c.depth_doses_) {
        multiplied_depth_doses.push_back(p * depth_dose);
    }
    return DepthDoseCurve(multiplied_depth_doses);
}

DepthDoseCurve operator/(const DepthDoseCurve& c, const double& p) {
    if (calc::AlmostEqual(p, 0.0, 0.0001)) {
        throw std::runtime_error("Cannot divide a DepthDoseCurve with zero");
    }

   return ((1.0 / p) * c);
}

// Returns the last element in the curve larger than or equal to [value]
DepthDose DepthDoseCurve::UpperRange(const double& dose) const {
    if (depth_doses_.empty()) {
        throw std::runtime_error("Trying to take upper range of empty depthdosecurve");
    }
    auto comp = [] (const DepthDose& d1, const DepthDose& d2) -> bool { return (d1.dose() < d2.dose()); };
    auto upper_range(std::lower_bound(depth_doses_.rbegin(), depth_doses_.rend(), DepthDose(0.0, dose), comp));
    if (upper_range != depth_doses_.rend()) {
        return *upper_range;
    } else {
        return *depth_doses_.begin();
    }
}

// Returns the first element in the curve having a dose larger than or equal to [value]
DepthDose DepthDoseCurve::LowerRange(const double& dose) const {
    if (depth_doses_.empty()) {
        throw std::runtime_error("Trying to take lower range of empty depthdosecurve");
    }
    auto comp = [] (const DepthDose& d1, const DepthDose& d2) -> bool
    { return (d1.dose() < d2.dose()); };
    auto lower_range(std::lower_bound(depth_doses_.begin(), depth_doses_.end(), DepthDose(0.0, dose), comp));
    if (lower_range != depth_doses_.end()) {
        return *lower_range;
    } else {
        return *depth_doses_.rbegin();
    }
}

// returns the first element smaller (dose) than the previous element starting from left going right
DepthDose DepthDoseCurve::LeftMostMax() const {
    if ((int)depth_doses_.size() < 2) {
        throw std::runtime_error("Trying to find left most max of depthdosecurve with less than 2 entries");
    }
    for (auto it = (depth_doses_.begin() + 1); it < depth_doses_.end(); ++it) {
        if (it->dose() <= (it - 1)->dose() && it->dose() > 0.0001) return *(it - 1);
    }
    return *(depth_doses_.rbegin());
}

// returns the first element smaller (dose) than the previous element starting from right going left
DepthDose DepthDoseCurve::RightMostMax() const {
    if ((int)depth_doses_.size() < 2) {
        throw std::runtime_error("Trying to find right most max of depthdosecurve with less than 2 entries");
    }
    for (auto it = (depth_doses_.rbegin() + 1); it < depth_doses_.rend(); ++it) {
        if (it->dose() <= (it - 1)->dose() && it->dose() > 0.0001) return *(it - 1);
    }
    return *(depth_doses_.begin());
}

double DepthDoseCurve::DoseDegradationFactor(double depth_shift) const {
    return (1.0084707 - (0.12231932 * depth_shift) + (0.0070574773 * std::pow(depth_shift, 2))
            - 0.00020157541 * std::pow(depth_shift, 3) + (0.00000218068 * std::pow(depth_shift, 4)));
}

// returns the depth of the first position on the curve with the dose = [dose] starting from right
double DepthDoseCurve::CorrespondingDepthFromRight(double dose) const {
    if (dose < depth_doses_.rbegin()->dose()) {
        return depth_doses_.rbegin()->depth();
    }
    for (auto it = (depth_doses_.rbegin() + 1); it != depth_doses_.rend(); ++it) {
        if (it->dose() >= dose) {
            DepthDose upper_item(*it);
            DepthDose lower_item(*(it - 1));
            return (dose - upper_item.dose()) * (upper_item.depth() - lower_item.depth()) / (upper_item.dose() - lower_item.dose())
                    + upper_item.depth();
        }
    }
    return depth_doses_.begin()->depth();
}

double DepthDoseCurve::MaxParcours() const {
    double d10(0.9 * RightMostMax().dose());
    double max_parcours(CorrespondingDepthFromRight(d10));
    return max_parcours;
}

double DepthDoseCurve::Penumbra() const {
    double d10(0.1 * RightMostMax().dose());
    double d90(0.9 * RightMostMax().dose());
    double d90_depth(this->CorrespondingDepthFromRight(d90));
    double d10_depth(this->CorrespondingDepthFromRight(d10));
    return d10_depth - d90_depth;
}

double DepthDoseCurve::Flatness() const {
    DepthDose left_most_max(LeftMostMax());
    DepthDose right_most_max(RightMostMax());
    std::vector<double> plateau_values;
    for (auto& d : depth_doses_) {
        if (d.depth() >= left_most_max.depth() && d.depth() <= right_most_max.depth()) {
            plateau_values.push_back(d.dose());
        }
    }
    const double std_dev(calc::StdDev(plateau_values));
    const double mean(calc::Mean(plateau_values));
    return 100.0 * (std_dev / mean);
}

DepthDoseCurve DepthDoseCurve::Degrade(const double& depth) const {
    std::vector<DepthDose> degraded_depth_doses;
    for (auto it = depth_doses_.begin(); it < depth_doses_.end(); ++it) {
        double shifted_depth(it->depth() + depth);
        if (InInterval(shifted_depth)) {
            degraded_depth_doses.push_back(DepthDose(it->depth(), this->Dose(shifted_depth).dose() * DoseDegradationFactor(depth)));
        } else {
            degraded_depth_doses.push_back(DepthDose(it->depth(), 0.0));
        }
    }
    return DepthDoseCurve(degraded_depth_doses);
}

bool operator==(const DepthDoseCurve& c1, const DepthDoseCurve& c2) {
    return (c1.depth_doses_ == c2.depth_doses_);
}

bool operator!=(const DepthDoseCurve& c1, const DepthDoseCurve& c2) {
    return !(c1 == c2);
}
