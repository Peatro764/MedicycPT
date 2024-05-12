
#include "Treatment.h"

#include <algorithm>
#include <iostream>
#include <QDebug>
#include <sstream>

#include "Calc.h"
#include "Util.h"

Treatment::Treatment(const int& dossier, const Patient& patient,
                     const int& active_seance_number,
                     const std::vector<Seance>& seances,
                     const Dosimetry& patientDosimetry,
                     const Dosimetry& patientRefDosimetry,
                     const Dosimetry& refDosimetryToday,
                     const double& dref,
                     const Modulateur& modulateur,
                     const DegradeurSet& degradeur_set,
                     QString treatment_type,
                     QString comments)
    : dossier_(dossier), patient_(patient),
      active_seance_number_(active_seance_number),
      seances_(seances), patientDosimetry_(patientDosimetry), patientRefDosimetry_(patientRefDosimetry),
      refDosimetryToday_(refDosimetryToday),
      dref_(dref),
      modulateur_(modulateur),
      degradeur_set_(degradeur_set),
      treatment_type_(treatment_type),
      comments_(comments) {}

Treatment::Treatment(const int& dossier,
                     const Patient& patient,
                     const std::vector<Seance>& seances)
    : dossier_(dossier), patient_(patient), active_seance_number_(0),
      seances_(seances), patientDosimetry_(Dosimetry()), patientRefDosimetry_(Dosimetry()),
      refDosimetryToday_(Dosimetry()), dref_(0.0f), modulateur_(Modulateur()), degradeur_set_(DegradeurSet()), treatment_type_("Unknown")
{}

Treatment::Treatment()
    : dossier_(0), patient_(Patient()), active_seance_number_(0),
      seances_(std::vector<Seance>()), patientDosimetry_(Dosimetry()), patientRefDosimetry_(Dosimetry()),
      refDosimetryToday_(Dosimetry()), dref_(0.0f), modulateur_(Modulateur()), degradeur_set_(DegradeurSet()), treatment_type_("Unknown") {}

Treatment::~Treatment() {}

SeanceConfig Treatment::GetSeanceConfig() {
    const QString name = patient_.GetLastName();
    const uint32_t dossier = static_cast<uint32_t>(dossier_);
    uint16_t seance_number = 0;
    const  float dose = static_cast<float>(GetRemainingDoseActiveSeance());
    float debit = 0.0;
    uint32_t mu_des = 0;
    const double des_duration = 10.0;
    double act_duration = 0.0;
    double i_stripper = 0.0;

    if (!IsFinished()) {
        seance_number = static_cast<uint16_t>(this->GetActiveSeanceNumber() + 1);
        if (BeamCalibrationDone()) {
            debit = static_cast<float>(this->GetBeamCalibration());
            mu_des = static_cast<uint32_t>(this->GetUMPrevuActiveSeance());
            util::GetStripperCurrentAndSeanceDuration(GetBeamCalibration(),
                                                      dose,
                                                      des_duration,
                                                      &i_stripper,
                                                      &act_duration);
        }
    }

    return SeanceConfig(name,
                        dossier,
                        seance_number,
                        dose,
                        debit,
                        mu_des,
                        0, // UM correction, now removed
                        static_cast<uint32_t>(1000.0 * act_duration),
                        static_cast<uint32_t>(i_stripper));
}

bool Treatment::IsFinished() const {
    return (active_seance_number_ == -1);
}

Seance* Treatment::GetActiveSeance() {
    if (active_seance_number_ != -1) {
        return &(seances_.at(active_seance_number_));
    } else {
        return nullptr;
    }
}

Seance Treatment::GetSeance(int index) const {
    if (index >= (int)seances_.size() || index < 0) {
        throw std::runtime_error("Treatment::GetSeance Index out of bounds");
    }
    return seances_.at(index);
}

std::vector<Seance> Treatment::GetPastSeances() {
    if (active_seance_number_ == -1) {
        return seances_;
    } else {
        auto begin = seances_.begin();
        auto last = seances_.begin() + active_seance_number_;
        std::vector<Seance> pastSeances(begin, last);
        return pastSeances;
    }
}

std::vector<Seance> Treatment::GetFutureSeances() {
    bool all_seances_done(active_seance_number_ == -1);
    bool last_seance(active_seance_number_ == (int)(seances_.size() - 1));
    if (all_seances_done || last_seance || seances_.empty()) {
        std::vector<Seance> noSeances;
        return noSeances;
    } else {
        auto begin = seances_.begin() + active_seance_number_ + 1;
        auto last = seances_.end();
        std::vector<Seance> futureSeances(begin, last);
        return futureSeances;
    }
}

bool Treatment::IsLastSeance() const {
    return (active_seance_number_ == (int)(seances_.size() - 1));
}

void Treatment::Close() {
    active_seance_number_ = -1;
}

void Treatment::IncrementSeance() {
    bool last_seance = (active_seance_number_ == (int)(seances_.size() - 1));
    if (last_seance) {
        active_seance_number_ = -1;
        return;
    }

    bool no_more_seances = (active_seance_number_ == -1);
    if (no_more_seances) {
        return;
    }

    ++active_seance_number_;
}

int Treatment::GetNmbRecords() const {
    int records(0);
    for (auto seance : seances_) {
        records += seance.GetSeanceRecords().size();
    }
    return records;
}

double Treatment::GetRemainingDoseActiveSeance() {
    double sum_dose_prescribed = 0.0;
    double sum_dose_delivered = 0.0;

    for (auto seance: this->GetPastSeances()) {
        sum_dose_prescribed += seance.GetDosePrescribed();
        sum_dose_delivered += seance.GetDoseDelivered();
    }

    if (this->GetActiveSeance()) {
        sum_dose_prescribed += this->GetActiveSeance()->GetDosePrescribed();
        sum_dose_delivered += this->GetActiveSeance()->GetDoseDelivered();
    }

    return std::max(0.0, sum_dose_prescribed - sum_dose_delivered);
}

double Treatment::GetRemainingDoseDossier() {
    return (this->GetPrescribedDose() - this->GetDeliveredDose());
}

double Treatment::GetDesiredDoseActiveSeance() {
    if (this->GetActiveSeance()) {
        double sum_dose_prescribed = 0.0;
        double sum_dose_delivered = 0.0;

        for (auto seance: this->GetPastSeances()) {
            sum_dose_prescribed += seance.GetDosePrescribed();
            sum_dose_delivered += seance.GetDoseDelivered();
        }

        sum_dose_prescribed += this->GetActiveSeance()->GetDosePrescribed();
        return std::max(0.0, sum_dose_prescribed - sum_dose_delivered);
    } else {
        return 0.0;
    }

}

int Treatment::GetUMPrevuActiveSeance()  {
    return static_cast<int>(std::round(calc::DoseToUM(GetBeamCalibration(), GetRemainingDoseActiveSeance())));
}

double Treatment::GetDeliveredDose() {
    double cum_dose(0.0);
    for (auto seance : this->GetAllSeances()) {
        cum_dose += seance.GetDoseDelivered();
    }

    return cum_dose;
}

double Treatment::GetEstimatedDeliveredDose() {
    double cum_dose(0.0);
    for (auto seance : this->GetAllSeances()) {
        cum_dose += seance.GetDoseEstimated();
    }

    return cum_dose;
}

double Treatment::GetPrescribedDose() {
    double cum_dose(0.0);
    for (auto seance : this->GetAllSeances()) {
        cum_dose += seance.GetDosePrescribed();
    }

    return cum_dose;
}

double Treatment::GetBeamCalibration() const {
    if (BeamCalibrationDone()) {
        if (patientDosimetry_.IsMonteCarlo()) {
            return (patientDosimetry_.GetDebitMean() * refDosimetryToday_.GetDebitMean() / dref_);
        } else {
            return (patientDosimetry_.GetDebitMean() * refDosimetryToday_.GetDebitMean() / patientRefDosimetry_.GetDebitMean() );
        }
    } else {
        return 0.0;
    }
}

double Treatment::GetBeamCalibrationStdDev() const {
    if (BeamCalibrationDone()) {
        if (patientDosimetry_.IsMonteCarlo()) {
            return GetBeamCalibration() * sqrt(pow(refDosimetryToday_.GetDebitStdDev() / refDosimetryToday_.GetDebitMean(), 2.0));
        } else {
            double squaredSum(0.0);
            squaredSum += pow(patientDosimetry_.GetDebitStdDev() / patientDosimetry_.GetDebitMean(), 2.0);
            if (refDosimetryToday_.GetTimestamp() != patientRefDosimetry_.GetTimestamp()) {
                squaredSum += pow(refDosimetryToday_.GetDebitStdDev() / refDosimetryToday_.GetDebitMean(), 2.0) +
                      pow(patientRefDosimetry_.GetDebitStdDev() / patientRefDosimetry_.GetDebitMean(), 2.0);
            }
            return (GetBeamCalibration() * sqrt(squaredSum));
        }
    } else {
        return 0.0;
    }
}

bool Treatment::BeamCalibrationDone() const {
    // if montecarlo -> dref is patientrefdosimetry
    bool patientRefDosimetryExist(patientRefDosimetry_.IsValid() || patientDosimetry_.IsMonteCarlo());
    return (patientDosimetry_.IsValid() &&  patientRefDosimetryExist && refDosimetryToday_.IsValid());
}

QString Treatment::ToPlainText() {
    std::stringstream plainText;
    plainText << "Beam shaping\n";
    plainText << "Modulateur: \t" + modulateur_.id().toStdString() << "\t";
    plainText << "Degradeurs: \t" + degradeur_set_.mm_plexis().join(", ").toStdString() << std::endl;
    plainText << std::endl;

    plainText << "Beam Calibration\n";
    if (patientDosimetry_.IsMonteCarlo()) {
        plainText << "MC Patient dosimetry:   " << patientDosimetry_.GetTimestamp().toString().toStdString() << "  débit (dref, cGy/UM) = " << QString::number(patientDosimetry_.GetDebitMean(), 'f', 4).toStdString() + " +- " << QString::number(patientDosimetry_.GetDebitStdDev(), 'f', 4).toStdString() << std::endl;
    } else {
        plainText << "Patient dosimetry:   " << patientDosimetry_.GetTimestamp().toString().toStdString() << "  débit (cGy/UM) = " << QString::number(patientDosimetry_.GetDebitMean(), 'f', 4).toStdString() + " +- " << QString::number(patientDosimetry_.GetDebitStdDev(), 'f', 4).toStdString() << std::endl;
    }
    plainText << "Patient top du jour: " << patientRefDosimetry_.GetTimestamp().toString().toStdString() << "  débit (cGy/UM) = " << QString::number(patientRefDosimetry_.GetDebitMean(), 'f', 4).toStdString() << " +- " << QString::number(patientRefDosimetry_.GetDebitStdDev(), 'f', 4).toStdString() << std::endl;
    plainText << "Todays top du jour:  " << refDosimetryToday_.GetTimestamp().toString().toStdString() << "  débit (cGy/UM) = " << QString::number(refDosimetryToday_.GetDebitMean(), 'f', 4).toStdString() << " +- " << QString::number(refDosimetryToday_.GetDebitStdDev(), 'f', 4).toStdString() << std::endl;
    plainText << "Todays debit (cGy/UM): \t" << GetBeamCalibration() << " +- " << GetBeamCalibrationStdDev() << std::endl;
    plainText << std::endl;

    plainText << "Treatment Summary\n";
    plainText << "Total delivered dose:  " << this->GetDeliveredDose() << " Gy" << std::endl;
    plainText << "Total prescribed dose: " << this->GetPrescribedDose() << " Gy" << std::endl;
    plainText << std::endl;

    int seanceNumber(1);
    for (Seance seance : seances_) {
        plainText << "Seance " << seanceNumber++ << std::endl;
        plainText << seance;
    }


    return QString::fromStdString(plainText.str());
}




