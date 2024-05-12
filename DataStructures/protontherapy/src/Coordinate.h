#ifndef COORDINATE_H
#define COORDINATE_H

class Coordinate {
public:
    Coordinate (float x, float y) : x_(x), y_(y) {}
    float x() const { return x_; }
    float y() const { return y_; }
private:
    float x_;
    float y_;
};

bool operator==(const Coordinate &c1, const Coordinate& c2);

#endif // COORDINATE_H
