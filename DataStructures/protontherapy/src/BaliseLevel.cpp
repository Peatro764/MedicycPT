#include "BaliseLevel.h"

#include "Calc.h"

BaliseLevel::BaliseLevel(const QDateTime &timestamp,
                         double value) :
    timestamp_(timestamp),
    value_(value) {}

BaliseLevel::~BaliseLevel() {}

BaliseInstantaneousLevel::BaliseInstantaneousLevel(const QDateTime &timestamp, double value)
    : BaliseLevel(timestamp, value) {}

BaliseIntegratedLevel::BaliseIntegratedLevel(const QDateTime &timestamp, double value)
    : BaliseLevel(timestamp, value) {}

BaliseBufferLevel::BaliseBufferLevel(const QDateTime &timestamp, double value)
    : BaliseLevel(timestamp, value) {}


bool operator==(const BaliseLevel &b1, const BaliseLevel& b2) {
    return (b1.GetTimestamp() == b2.GetTimestamp() &&
            calc::AlmostEqual(b1.GetValue(), b2.GetValue(), 0.0001));
}

bool operator!=(const BaliseLevel &b1, const BaliseLevel& b2) {
    return !(b1 == b2);
}


