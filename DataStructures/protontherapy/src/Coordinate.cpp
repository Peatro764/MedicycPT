#include "Coordinate.h"

#include "Calc.h"

bool operator==(const Coordinate &c1, const Coordinate& c2) {
    return (calc::AlmostEqual(c1.x(), c2.x(), 0.00001) &&
            calc::AlmostEqual(c1.y(), c2.y(), 0.00001));
}


