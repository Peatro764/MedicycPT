#include "Seance.h"

#include <iostream>

#include "Calc.h"

Seance::Seance(const int& seance_id,
               const double& dose_prescribed,
               const std::list<SeanceRecord>& seance_records)
    : seance_id_(seance_id),
      dose_prescribed_(dose_prescribed),
      seance_records_(seance_records) {}

Seance::Seance()
    : seance_id_(0),
      dose_prescribed_(0.0) {}

Seance::~Seance() {}

double Seance::GetDoseDelivered() const {
    double sum_dose(0.0);
    for (auto record : seance_records_) {
        sum_dose += record.GetDoseDelivered();
    }
    return sum_dose;
}

double Seance::GetDoseEstimated() const {
    double sum_dose(0.0);
    for (auto record : seance_records_) {
        sum_dose += record.GetDoseEstimated();
    }
    return sum_dose;
}

void Seance::AddSeanceRecord(const SeanceRecord& record) {
    seance_records_.push_back(record);
}

std::ostream& operator<< (std::ostream &out, const Seance &seance) {
    out << "Dose prescribed: " << seance.GetDosePrescribed() << " Gy \t";
    out << "Dose delivered: " << seance.GetDoseDelivered() << " Gy" << std::endl;
    auto records = seance.GetSeanceRecords();
    if (!records.empty()) {
        out << "Timestamp\t\tDuration\tDébit\tUM_pre\tUM_del" << std::endl;
        for (auto record : records) {
            out << record;
        }
    } else {
        out << "No delivered beams" << std::endl;
    }
    out << std::endl;
    return out;
}

bool operator==(const Seance& s1, const Seance& s2) {
    return (calc::AlmostEqual(s1.GetDosePrescribed(), s2.GetDosePrescribed(), 0.001) &&
            s1.GetSeanceId() == s2.GetSeanceId() &&
            s1.GetSeanceRecords() == s2.GetSeanceRecords());
}

bool operator!=(const Seance& s1, const Seance& s2) {
    return !(s1 == s2);
}


