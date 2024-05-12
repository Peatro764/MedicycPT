#include "Sensor.h"

#include <QDebug>

#include "Calc.h"

Sensor::Sensor(std::shared_ptr<LabJack> labjack, const SensorConfig& config)
    : labjack_(labjack), config_(config) {}

Sensor::~Sensor() {}

void Sensor::Configure() {
    labjack_->ConfigureAIN(config_.Ch(),
                           config_.SingleEndedChannel(),
                           config_.Range(),
                           config_.ResolutionIndex(),
                           config_.SettlingUs());
    configured_ = true;
}

void Sensor::ConfigureRapidScan(int samples, int rate) {
    if (samples < 1 || rate < 1) {
        throw std::runtime_error("Non valid rapid scan parameters");
    }
    labjack_->ConfigureRapidAINScan(config_.Ch(), samples, rate);
}

void Sensor::DisableRapidScan() {
    labjack_->DisableRapidAINScan(config_.Ch());
}

double Sensor::ReadRapidScan() {
    return labjack_->ReadRapidAINScan(config_.Ch());
}

double Sensor::RawValue() {
    if (!configured_) {
        throw std::runtime_error("Sensor must be configured before being used");
    }
    return labjack_->ReadAIN(config_.Ch());
}

double Sensor::InterpretedValue() {
    return RawValue();
}

double Sensor::InterpretedValue(int n_measurements) {
    auto raw = MultipleRawValue(n_measurements);
    return calc::Mean(raw);
}

double Sensor::InterpretedValue(int n_measurements, double n_sigma_filter) {
    auto raw = MultipleRawValue(n_measurements);
    auto filtered = calc::Filter(raw, n_sigma_filter);
    if (filtered.empty()) {
        return calc::Mean(raw);
    } else {
        return calc::Mean(filtered);
    }
}

std::vector<double> Sensor::MultipleRawValue(int n) {
    std::vector<double> values;
    for (int i = 0; i < n; ++i) {
        values.push_back(RawValue());
    }
    return values;
}

