#include "Range.h"

Range::Range()
    : start_(0.0), end_(0.0) {}

Range::Range(double start, double end)
    : start_(start), end_(end) {}

Range::~Range() {}

bool Range::IsInside(double value) const {
    return (value >= start_ && value <= end_) ||
            (value <= start_ && value >= end_);
}

void Range::invert() {
    const double tmp = start_;
    start_ = end_;
    end_ = tmp;
}

//bool Range::ReachedLimit(double value, double delta_value) const {
//    bool reached_lower_limit = (value <= (start_ + 0.01) && delta_value < 0.0);
//    bool reached_upper_limit = (value >= (end_ - 0.01) && delta_value > 0.0);
//    return (reached_lower_limit || reached_upper_limit);
//}
