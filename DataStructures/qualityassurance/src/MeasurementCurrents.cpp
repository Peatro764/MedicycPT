#include "MeasurementCurrents.h"

#include "Calc.h"

bool operator==(const MeasurementCurrents& c1, const MeasurementCurrents& c2) {
    return (calc::AlmostEqual(c1.cf9(), c2.cf9(), 0.0001) &&
            calc::AlmostEqual(c1.chambre1(), c2.chambre1(), 0.0001) &&
            calc::AlmostEqual(c1.chambre2(), c2.chambre2(), 0.0001) &&
            calc::AlmostEqual(c1.diode(), c2.diode(), 0.0001) &&
            calc::AlmostEqual(c1.stripper(), c2.stripper(), 0.0001) &&
            c1.timestamp() == c2.timestamp());
}

bool operator!=(const MeasurementCurrents& c1, const MeasurementCurrents& c2) {
    return !(c1 == c2);
}
