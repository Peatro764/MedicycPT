#include "Eye.h"

#include <QtMath>
#include <stdexcept>
#include <QDebug>

#include "Calc.h"

Eye::Eye() {}

Eye::Eye(double diameter, double sclere_thickness) :
    diameter_(diameter), sclere_thickness_(sclere_thickness)
{
    if (diameter_ <= 0.0) { throw std::runtime_error("Eye requires a positive diameter"); }
    if (diameter_ <= 2.0 * sclere_thickness_) { throw std::runtime_error("Eye diameter must be larger than double sclere thickness"); }
}

QCPDataContainer<QCPCurveData> Eye::EyeCurve(double r1, double r2) const {
    return MakeCircle(diameter_ / 2.0, r1, r2);
}

QCPDataContainer<QCPCurveData> Eye::SclereCurve(double r1, double r2) const {
    return MakeCircle(sclere_radius(), r1, r2);
}

QCPDataContainer<QCPCurveData> Eye::MakeCircle(double radius, double r1, double r2) const {
    const double delta_rad(fabs(r1 - r2) / 500);
    QCPDataContainer<QCPCurveData> data;
    if (r1 > r2) {
        for (double angle = r1; angle >= r2; angle -= delta_rad) {
            data.add(QCPCurveData(r1 - angle, radius * qCos(angle), radius * qSin(angle)));
        }
    } else {
        for (double angle = r1; angle <= r2; angle += delta_rad) {
            data.add(QCPCurveData(angle, radius * qCos(angle), radius * qSin(angle)));
        }
    }
    return data;
}

double Eye::sclere_radius() const {
     return (diameter_ / 2.0 - sclere_thickness_);
}

bool operator==(const Eye &e1, const Eye& e2) {
    return (calc::AlmostEqual(e1.diameter(), e2.diameter(), 0.001) &&
            calc::AlmostEqual(e1.sclere_thickness(), e2.sclere_thickness(), 0.001));

}

bool operator!=(const Eye &e1, const Eye& e2) {
    return !(e1 == e2);
}
