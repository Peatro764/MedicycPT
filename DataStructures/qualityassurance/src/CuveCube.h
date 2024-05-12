#ifndef QUALITYASSURANCE_CUVECUBE_H
#define QUALITYASSURANCE_CUVECUBE_H

#include <map>

#include "CuveSlice.h"
#include "qcustomplot.h"
#include "Range.h"
#include "Axis.h"
#include "BeamSignal.h"
#include "MeasurementPoint.h"

#include <QDateTime>

class vector3d {
public:
    vector3d(size_t d1 = 0, size_t d2 = 0, size_t d3 = 0) :
        d1_(d1), d2_(d2), d3_(d3), data_(d1 * d2 * d3)
    {}

    BeamSignal & operator()(size_t i, size_t j, size_t k) {
        return data_[i * d2_* d3_ + j * d3_ + k];
    }

    BeamSignal const & operator()(size_t i, size_t j, size_t k) const {
        return data_[i *d2_ * d3_ + j * d3_ + k];
    }
    void Clear() { data_.clear(); }
    std::vector<BeamSignal> data() const { return data_; }

private:
    size_t d1_, d2_, d3_;
    std::vector<BeamSignal> data_;
};

class AxisConfig {
public:
    AxisConfig()
        : nbins_(0), min_(0.0), step_(0) {}
    AxisConfig(int nbins, double min, double step)
        : nbins_(nbins), min_(min), step_(step) {}
    int nbins() const { return nbins_; }
    double step() const { return step_; }
    double min() const { return min_; }
    double max() const { return min_ + (nbins_ - 1) * step_; }
    QCPRange qcprange() const { return QCPRange(min(), max()); }
    Range range() const { return Range(min(), max()); }

private:
    int nbins_ = 0;
    double min_ = 0.0;
    double step_ = 0.0;
};

bool operator==(const AxisConfig& c1, const AxisConfig& c2);

enum class PROJECTION : int { XY = 0, ZX = 1, YZ = 2 };

class CuveCube
{
public:
    CuveCube();
    CuveCube(QDateTime timestamp, QString comment,
             std::map<Axis, AxisConfig> configs,
             std::vector<MeasurementPoint> points,
             double noise);
    CuveCube(std::map<Axis, AxisConfig> configs, double noise);
    void Add(MeasurementPoint p);
    void Add(int xbin, int ybin, int zbin, const BeamSignal& signal);
    void Clear() { signal_.Clear(); }
    int nbins(Axis axis) const;
    double pos(Axis axis, int bin) const;
    std::vector<MeasurementPoint> GetMeasurementPoints() const;
    QString comment() const { return comment_; }
    void SetComment(QString comment) { comment_ = comment; }
    QDateTime timestamp() const { return timestamp_; }
    void SetTimestamp(QDateTime timestamp) { timestamp_ = timestamp; }
    AxisConfig GetAxisConfig(Axis axis) const;
    std::map<Axis, AxisConfig> GetAxisConfigs() const;
    double GetNoise() const { return signal_noise_; }
    void SetNoise(double noise) { signal_noise_ = noise; }

    QCPColorMapData GetColorMap(PROJECTION proj, int slice) const;

private:
    int CoordToBin(double value, Axis axis) const;
    double BinToCoord(int bin, Axis axis) const;
    friend bool operator==(const CuveCube& c1, const CuveCube& c2);
    friend bool operator!=(const CuveCube& c1, const CuveCube& c2);

    QDateTime timestamp_;
    QString comment_ = "default";
    std::map<Axis, AxisConfig> configs_;
    vector3d signal_;
    double signal_noise_ = 0.0;
};

bool operator==(const CuveCube& c1, const CuveCube& c2);
bool operator!=(const CuveCube& c1, const CuveCube& c2);


#endif
