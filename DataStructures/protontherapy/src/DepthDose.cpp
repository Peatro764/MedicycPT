#include "DepthDose.h"

#include "Calc.h"
#include <stdexcept>

DepthDose::DepthDose(const double& depth, const double& dose) :
    depth_(depth), dose_(dose) {
    if (depth < 0.0) {
        throw std::runtime_error("A DepthDose cannot have a negative depth");
    }
}

DepthDose operator+(const DepthDose& d1, const DepthDose& d2) {
    if (!calc::AlmostEqual(d1.depth(), d2.depth(), 0.0001)) {
        throw std::runtime_error("Trying to sum two depthdoses with different depth");
    }
    return DepthDose(d1.depth(), d1.dose() + d2.dose());
}

DepthDose operator-(const DepthDose& d) {
    return DepthDose(d.depth(), -d.dose());
}

DepthDose operator-(const DepthDose& d1, const DepthDose& d2) {
    if (!calc::AlmostEqual(d1.depth(), d2.depth(), 0.0001)) {
        throw std::runtime_error("Trying to subtract two depthdoses with different depth");
    }
    return DepthDose(d1.depth(), d1.dose() - d2.dose());
}

DepthDose operator*(const DepthDose& d, const double& p) {
    return DepthDose(d.depth(), p * d.dose());
}

DepthDose operator*(const double& p, const DepthDose& d) {
    return DepthDose(d.depth(), p * d.dose());
}

bool operator==(const DepthDose& d1, const DepthDose& d2) {
    return (calc::AlmostEqual(d1.depth(), d2.depth(), 0.0001) &&
            calc::AlmostEqual(d1.dose(), d2.dose(), 0.0001));
}

bool operator!=(const DepthDose& d1, const DepthDose& d2) {
    return !(d1 == d2);
}
