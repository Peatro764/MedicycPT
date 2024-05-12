#ifndef PROTONTHERAPIE_BALISELEVEL_H
#define PROTONTHERAPIE_BALISELEVEL_H

#include <QDateTime>

class BaliseLevel {
public:
    BaliseLevel(const QDateTime& timestamp, double value);
    ~BaliseLevel();
    QDateTime GetTimestamp() const { return timestamp_; }
    double GetValue() const { return value_; }

private:
    QDateTime timestamp_;
    double value_;
};

class BaliseInstantaneousLevel : public BaliseLevel {
public:
    BaliseInstantaneousLevel(const QDateTime& timestamp, double value);
};

class BaliseIntegratedLevel : public BaliseLevel {
public:
    BaliseIntegratedLevel(const QDateTime& timestamp, double value);
};

class BaliseBufferLevel : public BaliseLevel {
public:
    BaliseBufferLevel(const QDateTime& timestamp, double value);
};

 bool operator==(const BaliseLevel &b1, const BaliseLevel& b2);
 bool operator!=(const BaliseLevel &b1, const BaliseLevel& b2);

#endif
