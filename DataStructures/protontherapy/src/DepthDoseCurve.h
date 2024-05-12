#ifndef DEPTHDOSECURVE_H
#define DEPTHDOSECURVE_H

#include <vector>

#include "DepthDose.h"

class DepthDoseCurve
{
public:
    DepthDoseCurve();
    DepthDoseCurve(const std::vector<DepthDose>& depth_doses);
    DepthDose Dose(double depth) const;
    bool InInterval(double depth) const;
    DepthDose Max() const;
    DepthDose LeftMostMax() const;
    DepthDose RightMostMax() const;
    DepthDose UpperRange(const double& dose) const;
    DepthDose LowerRange(const double& dose) const;
    std::vector<DepthDose> DepthDoses() const { return depth_doses_; }
    DepthDoseCurve CapAtZero() const;
    DepthDoseCurve Degrade(const double& depth) const;
    double DoseDegradationFactor(double depth_shift) const;
    double CorrespondingDepthFromRight(double dose) const;
    double MaxParcours() const;
    double Penumbra() const;
    double Flatness() const;

private:
    void Sort();
    friend DepthDoseCurve operator+(const DepthDoseCurve& c1, const DepthDoseCurve& c2);
    friend DepthDoseCurve operator-(const DepthDoseCurve& c);
    friend DepthDoseCurve operator-(const DepthDoseCurve& c1, const DepthDoseCurve& c2);
    friend DepthDoseCurve operator*(const double& p, const DepthDoseCurve& c);
    friend DepthDoseCurve operator/(const DepthDoseCurve& c, const double& p);
    friend bool operator==(const DepthDoseCurve& c1, const DepthDoseCurve& c2);
    friend bool operator!=(const DepthDoseCurve& c1, const DepthDoseCurve& c2);
    std::vector<DepthDose> depth_doses_;
};

DepthDoseCurve operator+(const DepthDoseCurve& c1, const DepthDoseCurve& c2);
DepthDoseCurve operator-(const DepthDoseCurve& c);
DepthDoseCurve operator-(const DepthDoseCurve& c1, const DepthDoseCurve& c2);
DepthDoseCurve operator*(const double& p, const DepthDoseCurve& c);
DepthDoseCurve operator/(const DepthDoseCurve& c, const double& p);
bool operator==(const DepthDoseCurve& c1, const DepthDoseCurve& c2);
bool operator!=(const DepthDoseCurve& c1, const DepthDoseCurve& c2);


#endif // DEPTHDOSECURVE_H
