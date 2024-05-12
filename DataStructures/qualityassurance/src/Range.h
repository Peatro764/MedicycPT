#ifndef QUALITYASSURANCE_RANGE_H
#define QUALITYASSURANCE_RANGE_H

#include <QString>

class Range
{
public:    
    Range();
    Range(double start, double end);
    ~Range();
    bool IsInside(double value) const;
//    bool ReachedLimit(double value, double delta_value) const;
    double start() const { return start_; }
    double end() const { return end_; }
    double length() const { return end_ - start_; }
    void addToend(double a) { end_ += a; }
    void addTostart(double a) { start_ += a; }
    QString toString() const { return QString::number(start_, 'f', 2) + " " + QString::number(end_, 'f', 2); }
    void invert();

private:
    double start_;
    double end_;
};


#endif
