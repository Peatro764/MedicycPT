#include "MockSensor.h"

#include <QDebug>

MockSensor::MockSensor(std::queue<double> values)
    : values_(values) {}

MockSensor::~MockSensor() {

}

void MockSensor::Configure() {

}

double MockSensor::RawValue() {
    if (values_.empty()) {
        return 0.3;
    }
    double m = values_.front();
    values_.pop();
    return m;
}

double MockSensor::InterpretedValue() {
    return RawValue();
}

double MockSensor::InterpretedValue(int n_measurements) {
    (void)n_measurements;
    return RawValue();
}

double MockSensor::InterpretedValue(int n_measurements, double n_sigma_filter) {
    (void)n_measurements;
    (void)n_sigma_filter;
    return RawValue();
}

std::vector<double> MockSensor::MultipleRawValue(int n) {
    std::vector<double> values;
    for (int i = 0; i < n; ++i) {
        values.push_back(RawValue());
    }
    return values;
}
