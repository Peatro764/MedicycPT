#include "PositionSensor.h"

#include <QDebug>

#include "Calc.h"

PositionSensor::PositionSensor(std::shared_ptr<LabJack> labjack, const SensorConfig& config)
    : labjack_(labjack), config_(config) {
}

PositionSensor::~PositionSensor() {}

void PositionSensor::Configure() {
    labjack_->ConfigureAIN(config_.Ch(),
                           config_.SingleEndedChannel(),
                           config_.Range(),
                           config_.ResolutionIndex(),
                           config_.SettlingUs());
    configured_ = true;
}

void PositionSensor::ConfigureRapidScan(int samples, int rate) {
    if (samples < 1 || rate < 1) {
        throw std::runtime_error("Non valid rapid scan parameters");
    }
    labjack_->ConfigureRapidAINScan(config_.Ch(), samples, rate);
}

void PositionSensor::DisableRapidScan() {
    labjack_->DisableRapidAINScan(config_.Ch());
}

double PositionSensor::ReadRapidScan() {
    return labjack_->ReadRapidAINScan(config_.Ch());
}

double PositionSensor::RawValue() {
    if (!configured_) {
        throw std::runtime_error("Sensor must be configured before being used");
    }
    return labjack_->ReadAIN(config_.Ch());
}

double PositionSensor::InterpretedValue() {
    return config_.UnitPerVolt() * RawValue() + config_.UnitOffset();
}

double PositionSensor::InterpretedValue(int n_measurements) {
    auto raw = MultipleRawValue(n_measurements);
    return calc::Median(raw);
}

double PositionSensor::InterpretedValue(int n_measurements, double n_sigma_filter) {
    auto raw = MultipleRawValue(n_measurements);
    auto filtered = calc::Filter(raw, n_sigma_filter);
    double mean(0.0);
    double sigma(0.0);
    if (filtered.empty()) {
        mean = calc::Mean(raw);
        sigma = calc::StdDev(raw);
    } else {
        mean = calc::Mean(filtered);
        sigma = calc::StdDev(filtered);
    }
    qDebug() << "PositionSensor::InterpretedValue (Mean, Sigma) " << mean << " " << sigma;
    return config_.UnitPerVolt() * mean + config_.UnitOffset();
}

std::vector<double> PositionSensor::MultipleRawValue(int n) {
    std::vector<double> values;
    for (int i = 0; i < n; ++i) {
        values.push_back(RawValue());
    }
    return values;
}

