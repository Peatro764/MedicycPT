#ifndef EYE_H
#define EYE_H

#include <math.h>

#include <QObject>

#include "qcustomplot.h"

class Eye
{
public:
    Eye();
    Eye(double diameter, double sclere_thickness);
    QCPDataContainer<QCPCurveData> EyeCurve(double r1, double r2) const;
    QCPDataContainer<QCPCurveData> SclereCurve(double r1, double r2) const;

    double diameter() const { return diameter_; }
    double sclere_thickness() const { return sclere_thickness_; }
    double sclere_radius() const;

private:
    QCPDataContainer<QCPCurveData> MakeCircle(double radius, double r1, double r2) const;
    double diameter_;
    double sclere_thickness_;
//    const double delta_rad = 2.0 * M_PI / 1000.0;
};

bool operator==(const Eye &e1, const Eye& e2);
bool operator!=(const Eye &e1, const Eye& e2);

#endif // EYE_H
