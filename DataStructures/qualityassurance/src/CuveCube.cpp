#include "CuveCube.h"

#include "qcustomplot.h"
#include "Calc.h"

CuveCube::CuveCube() {
    configs_[Axis::X] = AxisConfig();
    configs_[Axis::Y] = AxisConfig();
    configs_[Axis::Z] = AxisConfig();
}

CuveCube::CuveCube(QDateTime timestamp, QString comment,
                   std::map<Axis, AxisConfig> configs,
                   std::vector<MeasurementPoint> points,
                   double noise)
    : timestamp_(timestamp), comment_(comment),
      configs_(configs), signal_(configs.at(Axis::Z).nbins(),
                                 configs.at(Axis::Y).nbins(),
                                 configs.at(Axis::X).nbins()),
      signal_noise_(noise) {
    for (auto m : points) Add(m);
}

CuveCube::CuveCube(std::map<Axis, AxisConfig> configs, double noise)
    : configs_(configs), signal_(configs[Axis::Z].nbins(),
                                 configs[Axis::Y].nbins(),
                                 configs[Axis::X].nbins()),
                                 signal_noise_(noise) {


}

void CuveCube::Add(MeasurementPoint m) {
  Point p = m.point();
  const int x_bin = CoordToBin(p.value(Axis::X), Axis::X);
  const int y_bin = CoordToBin(p.value(Axis::Y), Axis::Y);
  const int z_bin = CoordToBin(p.value(Axis::Z), Axis::Z);
  Add(x_bin, y_bin, z_bin, m.signal());
}

void CuveCube::Add(int xbin, int ybin, int zbin, const BeamSignal &signal) {
  if (xbin >= configs_.at(Axis::X).nbins() || xbin < 0 ||
      ybin >= configs_.at(Axis::Y).nbins() || ybin < 0 ||
      zbin >= configs_.at(Axis::Z).nbins() || zbin < 0) {
      qDebug() << "CuveCube::Add Excep";
      throw std::runtime_error("CuveCube bin outside limits");
    }
  signal_(zbin, ybin, xbin) = signal;
}

int CuveCube::nbins(Axis axis) const {
    return configs_.at(axis).nbins();
}

double CuveCube::pos(Axis axis, int bin) const {
    if (bin >= configs_.at(axis).nbins()) {
        qDebug() << "CuveCube::pos bin out of range";
        throw std::runtime_error("CuveCube bin out of range");
    }
    return configs_.at(axis).min() + bin * configs_.at(axis).step();
}

std::vector<MeasurementPoint> CuveCube::GetMeasurementPoints() const {
    std::vector<MeasurementPoint> points;
    for (int x = 0; x < configs_.at(Axis::X).nbins(); ++x) {
        for (int y = 0; y < configs_.at(Axis::Y).nbins(); ++y) {
            for (int z = 0; z < configs_.at(Axis::Z).nbins(); ++z) {
                Point p(BinToCoord(x, Axis::X), BinToCoord(y, Axis::Y), BinToCoord(z, Axis::Z));
                points.push_back(MeasurementPoint(p, signal_(z, y, x)));
            }
        }
    }
    return points;
}

AxisConfig CuveCube::GetAxisConfig(Axis axis) const {
    return configs_.at(axis);
}

std::map<Axis, AxisConfig> CuveCube::GetAxisConfigs() const {
    std::map<Axis, AxisConfig> configs;
    configs[Axis::X] = configs_.at(Axis::X);
    configs[Axis::Y] = configs_.at(Axis::Y);
    configs[Axis::Z] = configs_.at(Axis::Z);
    return configs;
}

QCPColorMapData CuveCube::GetColorMap(PROJECTION proj, int slice) const {
    QCPColorMapData map(0, 0, QCPRange(0.0, 0.0), QCPRange(0.0, 0.0));

    switch (proj) {
    case PROJECTION::XY:
        if (slice >= configs_.at(Axis::Z).nbins()) {
            throw std::runtime_error("z-slice out of bounds in cuve cube");
        }
        map.setSize(configs_.at(Axis::X).nbins(), configs_.at(Axis::Y).nbins());
        map.setRange(configs_.at(Axis::X).qcprange(), configs_.at(Axis::Y).qcprange());
        for (int i_x = 0; i_x < configs_.at(Axis::X).nbins(); ++i_x) {
            for (int i_y = 0; i_y < configs_.at(Axis::Y).nbins(); ++i_y) {
                map.setCell(i_x, i_y, signal_(slice, i_y, i_x).value(signal_noise_));
            }
        }
        break;
    case PROJECTION::YZ:
        if (slice >= configs_.at(Axis::X).nbins()) {
            throw std::runtime_error("x-slice out of bounds in cuve cube");
        }
        map.setRange(configs_.at(Axis::Y).qcprange(), configs_.at(Axis::Z).qcprange());
        map.setSize(configs_.at(Axis::Y).nbins(), configs_.at(Axis::Z).nbins());
        for (int i_y = 0; i_y < configs_.at(Axis::Y).nbins(); ++i_y) {
            for (int i_z = 0; i_z < configs_.at(Axis::Z).nbins(); ++i_z) {
                map.setCell(i_y, i_z, signal_(i_z, i_y, slice).value(signal_noise_));
            }
        }
        break;
    case PROJECTION::ZX:
        if (slice >= configs_.at(Axis::Y).nbins()) {
            throw std::runtime_error("y-slice out of bounds in cuve cube");
        }
        map.setRange(configs_.at(Axis::Z).qcprange(), configs_.at(Axis::X).qcprange());
        map.setSize(configs_.at(Axis::Z).nbins(), configs_.at(Axis::X).nbins());
        for (int i_z = 0; i_z < configs_.at(Axis::Z).nbins(); ++i_z) {
            for (int i_x = 0; i_x < configs_.at(Axis::X).nbins(); ++i_x) {
                map.setCell(i_z, i_x, signal_(i_z, slice, i_x).value(signal_noise_));
            }
        }
        break;
    default:
        throw std::runtime_error("unknown cube projection");
        break;
    }

    return map;
}

bool operator==(const CuveCube& c1, const CuveCube& c2) {
    return c1.signal_.data() == c2.signal_.data() &&
            c1.timestamp_ == c2.timestamp_ &&
            c1.comment_ == c2.comment_ &&
            c1.configs_.at(Axis::X) == c2.configs_.at(Axis::X) &&
            c1.configs_.at(Axis::Y) == c2.configs_.at(Axis::Y) &&
            c1.configs_.at(Axis::Z) == c2.configs_.at(Axis::Z) &&
            calc::AlmostEqual(c1.GetNoise(), c2.GetNoise(), 0.001);
}

bool operator!=(const CuveCube& c1, const CuveCube& c2) {
    return !(c1 == c2);
}

int CuveCube::CoordToBin(double value, Axis axis) const {
    return std::min(configs_.at(axis).nbins() - 1, std::max(0, static_cast<int>(std::round((value - configs_.at(axis).min()) / configs_.at(axis).step()))));
}

double CuveCube::BinToCoord(int bin, Axis axis) const {
    return configs_.at(axis).min() + configs_.at(axis).step() * bin;
}

bool operator==(const AxisConfig& c1, const AxisConfig& c2) {
    return c1.nbins() == c2.nbins() &&
            calc::AlmostEqual(c1.step(), c2.step(), 0.0001) &&
            calc::AlmostEqual(c1.min(), c2.min(), 0.0001);
}

