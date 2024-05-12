#include "Point.h"

#include <QDebug>

#include "Calc.h"

Point::Point(Axis axis, double value) {
    values_[axis] = value;
}

Point::Point(double x, double y, double z) {
    values_[Axis::X] = x;
    values_[Axis::Y] = y;
    values_[Axis::Z] = z;
}

Point::Point() {
}

QString Point::toString() const {
    return "Point(x=" + QString::number(value(Axis::X), 'f', 2) +
            ", y=" + QString::number(value(Axis::Y), 'f', 2) +
            ", z=" + QString::number(value(Axis::Z), 'f', 2);
}

double Point::value(Axis axis) const {
    if (values_.find(axis) == values_.end()) {
        return 0.0;
    }
    return values_.at(axis);
}

bool operator==(const Point& p1, const Point& p2) {
    return (calc::AlmostEqual(p1.value(Axis::X), p2.value(Axis::X), 0.0001) &&
        calc::AlmostEqual(p1.value(Axis::Y), p2.value(Axis::Y), 0.0001) &&
        calc::AlmostEqual(p1.value(Axis::Z), p2.value(Axis::Z), 0.0001));
}

bool operator!=(const Point& p1, const Point& p2) {
    return !(p1 == p2);
}
