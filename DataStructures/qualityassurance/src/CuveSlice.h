#ifndef DATASTRUCTURES_CUVESLICE_H
#define DATASTRUCTURES_CUVESLICE_H

#include <vector>

#include "qcustomplot.h"
#include "Range.h"
#include "BeamSignal.h"

class CuveSlice
{
public:
    CuveSlice();

    void Add(double x, double y, const BeamSignal& signal);
    void Clear();
    double IntegratedSignal(const Range& x_range, const Range& y_range, double noise) const;
    double AverageSignal(const Range& x_range, const Range& y_range, double noise) const;
    QCPColorMapData GetColorMapData() const;
    QCPColorMapData GetColorMapData(int x_bins, int y_bins,
                                    Range x_range, Range y_range,
                                    double noise) const;

private:
    friend bool operator==(const CuveSlice& c1, const CuveSlice& c2);
    friend bool operator!=(const CuveSlice& c1, const CuveSlice& c2);

    std::vector<double> x_;
    std::vector<double> y_;
    std::vector<BeamSignal> signal_;
    int x_bins_ = 100;
    int y_bins_ = 100;
    Range x_range_ = Range(-20.0, 20.0);
    Range y_range_ = Range(-20.0, 20.0);
};

bool operator==(const CuveSlice& c1, const CuveSlice& c2);
bool operator!=(const CuveSlice& c1, const CuveSlice& c2);


#endif
