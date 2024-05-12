#ifndef QUALITYASSURANCE_POINT_H
#define QUALITYASSURANCE_POINT_H

#include "Axis.h"
#include <map>

class Point {
public:
    Point(double x, double y, double z);
    Point(Axis axis, double value);
    Point();

    double value(Axis axis) const;
    void set(Axis x, double pos) { values_[x] = pos; }
    QString toString() const;

private:
    std::map<Axis, double> values_;
};

bool operator==(const Point& p1, const Point& p2);
bool operator!=(const Point& p1, const Point& p2);

#endif
