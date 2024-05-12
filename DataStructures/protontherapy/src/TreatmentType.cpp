#include "TreatmentType.h"

#include <stdexcept>
#include <numeric>

#include "Calc.h"

TreatmentType::TreatmentType(QString name,
                             std::vector<double> dose_seance_std,
                             std::vector<double> dose_seance_boost)
    : name_(name), dose_seance_std_(dose_seance_std), dose_seance_boost_(dose_seance_boost) {}

TreatmentType::~TreatmentType() {

}

std::vector<double> TreatmentType::SeanceDoses() const {
    std::vector<double> combined = dose_seance_std_;
    combined.insert(combined.end(), dose_seance_boost_.begin(), dose_seance_boost_.end());
    return combined;
}

double TreatmentType::StandardDose() const {
    return std::accumulate(dose_seance_std_.begin(), dose_seance_std_.end(), 0.0);
}

double TreatmentType::StandardDoseEBR() const {
    return toEBR(StandardDose());
}

double TreatmentType::BoostDose() const {
    return std::accumulate(dose_seance_boost_.begin(), dose_seance_boost_.end(), 0.0);
}

double TreatmentType::BoostDoseEBR() const {
    return toEBR(BoostDose());
}

double TreatmentType::TotalDose() const {
    auto doses = this->SeanceDoses();
    return std::accumulate(doses.begin(), doses.end(), 0.0);
}

double TreatmentType::TotalDoseEBR() const {
    return toEBR(TotalDose());
}

QString TreatmentType::StandardSummary() const {
    if (dose_seance_std_.empty()) {
        return "-";
    } else {
        return VectorToString(dose_seance_std_) + " Gy";
    }
}

QString TreatmentType::BoostSummary() const {
    if (dose_seance_boost_.empty()) {
        return "-";
    } else {
        return VectorToString(dose_seance_boost_) + " Gy";
    }
}

QString TreatmentType::TotalSummary() const {
    return QString::number(TotalDose(), 'f', 2) + " Gy (" + QString::number(TotalDoseEBR(), 'f', 2) + " GyEBR)";
}

QString TreatmentType::VectorToString(std::vector<double> v) const {
    QString s("(");
    for (int idx = 0; idx < static_cast<int>(v.size()); ++idx) {
        s.append(QString::number(v.at(idx), 'f', 2) + (idx == static_cast<int>(v.size()) - 1 ? QString("") : QString(", ")));
    }
    return s.append(")");
}

void TreatmentType::AddStandard(double dose) {
    if (dose < 0.0001) {
        throw std::runtime_error("Cannot add a negative dose");
    }
    dose_seance_std_.push_back(dose);
}

void TreatmentType::AddBoost(double dose) {
    if (dose < 0.0001) {
        throw std::runtime_error("Cannot add a negative dose");
    }
    dose_seance_boost_.push_back(dose);
}


double TreatmentType::toEBR(double dose) const {
    return dose * 1.1;
}

bool operator==(const TreatmentType& t1, const TreatmentType& t2) {
    return t1.Name() == t2.Name() &&
            t1.SeanceDoses() == t2.SeanceDoses();
}

bool operator!=(const TreatmentType& t1, const TreatmentType& t2) {
    return !(t1 == t2);
}


