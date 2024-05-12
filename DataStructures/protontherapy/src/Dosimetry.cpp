#include "Dosimetry.h"

#include <QDebug>
#include <sstream>
#include <iomanip>

#include "Calc.h"   

Dosimetry::Dosimetry(const QDateTime& timestamp, const bool& reference, const bool& monte_carlo,
                     const double& temperature, const double& pressure, const Chambre& chambre,
                     const std::list<DosimetryRecord>& records, const QString& comment)
    : timestamp_(timestamp), reference_(reference), monte_carlo_(monte_carlo),
      temperature_(temperature), pressure_(pressure),
      chambre_(chambre), comment_(comment), records_(records), unsaved_changes_(false) {
    temperature_entered_ = true;
    pressure_entered_ = true;
    chambre_entered_ = true;
}

Dosimetry::Dosimetry()
    : timestamp_(QDateTime()), reference_(false), monte_carlo_(false),
      temperature_(20.0), pressure_(1013.0),
      chambre_(Chambre()), comment_(QString("")), records_(std::list<DosimetryRecord>()), unsaved_changes_(false) {}

Dosimetry::~Dosimetry() {}

void Dosimetry::HasBeenSaved() {
    unsaved_changes_ = false;
}

bool Dosimetry::UnSavedChanges() const {
    return (unsaved_changes_ && !records_.empty());
}

Debit Dosimetry::GetDebit() const {
    return Debit(GetDebitMean(),
                 GetDebitStdDev(),
                 GetTimestamp(),
                 IsMonteCarlo(),
                 GetChambre());
}

double Dosimetry::GetDebitMean() const {
    return calc::Mean(this->GetDebits());
}

double Dosimetry::GetDebitStdDev() const {
    return calc::StdDev(this->GetDebits());
}

std::vector<double> Dosimetry::GetRollingDebitMeans() const {
    auto debits = this->GetDebits();
    std::vector<double> means;
    for (auto it = debits.begin(); it != debits.end(); ++it) {
        std::vector<double> part(debits.begin(), it + 1);
        means.push_back(calc::Mean(part));
    }
    return means;
}

std::vector<double> Dosimetry::GetRollingDebitStdDevs() const {
    auto debits = this->GetDebits();
    std::vector<double> stdDevs;

    for (auto it = debits.begin(); it != debits.end(); ++it) {
        std::vector<double> part(debits.begin(), it + 1);
        stdDevs.push_back(calc::StdDev(part));
    }
    return stdDevs;
}

std::vector<double> Dosimetry::GetDebits() const {
    std::vector<double> debits;
    if (!records_.empty()) {
        for (DosimetryRecord record : records_) {
            debits.push_back(CalcCalibrationConstant(record.GetCharge(), record.GetTotalUMDelivered()));
        }
    }
    return debits;
}

double Dosimetry::GetFtp() const {
    if (pressure_ > 0.001 && temperature_entered_ && pressure_entered_) {
        return ((temperature_ + 273.15) / 293.15) * (1013.25 / pressure_);
    } else {
        qWarning() << "Dosimetry::GetFtp Temperature or pressure not given, or pressure is zero";
        return 0.0;
    }
}

void Dosimetry::SetTimestamp(const QDateTime& timestamp) {
    unsaved_changes_ = true;
    timestamp_ = timestamp;
}
void Dosimetry::SetReference(const bool& reference) {
    unsaved_changes_ = true;
    reference_ = reference;
}
void Dosimetry::SetMonteCarlo(const bool &monte_carlo) {
    unsaved_changes_ = true;
    monte_carlo_ = monte_carlo;
}

void Dosimetry::SetTemperature(const double& temperature) {
    unsaved_changes_ = true;
    temperature_entered_ = true;
    temperature_ = temperature;
}
void Dosimetry::SetPressure(const double& pressure) {
    unsaved_changes_ = true;
    pressure_entered_ = true;
    pressure_ = pressure;
}
void Dosimetry::SetChambre(const Chambre& chambre) {
    unsaved_changes_ = true;
    chambre_entered_ = true;
    chambre_ = chambre;
}
void Dosimetry::SetComment(const QString &comment) {
    unsaved_changes_ = true;
    comment_ = comment;
}

bool Dosimetry::TemperatureInRange(double n_stddev) const {
    const double mean = 21.99;
    const double stddev = 2.28;
    return ((temperature_ >= (mean - n_stddev *  stddev)) && (temperature_ <= (mean + n_stddev * stddev)));
}

bool Dosimetry::PressureInRange(double n_stddev) const {
    const double mean = 1007.7;
    const double stddev = 7.45;
    return ((pressure_ >= (mean - n_stddev *  stddev)) && (pressure_ <= (mean + n_stddev * stddev)));
}

void Dosimetry::AddRecord(const DosimetryRecord& record) {
    unsaved_changes_ = true;
    records_.push_back(record);
}

double Dosimetry::CalcCalibrationConstant(const double& charge, const int& um) const {
    if (um != 0 && this->MeasurementConditionsEntered()) {
        return (charge * this->GetFtp() * chambre_.nd() * chambre_.kqq0() / ((double)um));
    } else {
        qWarning() << "Dosimetry::CalcCalibrationConstant Temperature, pressure or chambre not given, or um = 0. Returning 0";
        return 0.0;
    }
}

double Dosimetry::GetExpectedCharge(const double& debit, const int& um) const {
    if (um < 0 || debit < 0.0 || !this->MeasurementConditionsEntered()) {
        qWarning() << "Dosimetry::GetExpectedCharge Non valid input, returning 0";
        return 0.0;
    }

    return (debit * ((double)um) / (GetFtp() * chambre_.nd() * chambre_.kqq0()));
}

void Dosimetry::DeleteSingleRecord(int index) {
    if (index < (int)records_.size()) {
        unsaved_changes_ = true;
        auto it = records_.begin();
        std::advance(it, index);
        records_.erase(it);
    }
}

QString Dosimetry::ToPlainText() const {
    std::stringstream plainText;
    plainText << *this;
    return QString::fromStdString(plainText.str());
}

bool Dosimetry::MeasurementConditionsEntered() const {
    return (temperature_entered_ && pressure_entered_ && chambre_entered_);
}

std::ostream& operator<< (std::ostream &out, const Dosimetry &dosimetry) {
    out << "Settings" << std::endl;
    out << "Timestamp\t " << dosimetry.GetTimestamp().toString("dd.MM.yyyy-hh:mm::ss").toStdString() << std::endl;
    out << "MonteCarlo\t " << dosimetry.IsMonteCarlo() << std::endl;
    out << "Temperature\t " << dosimetry.GetTemperature() << std::endl;
    out << "Pressure\t\t " << dosimetry.GetPressure() << std::endl;
    out << "Chambre\t\t " << dosimetry.GetChambre();
    out << std::endl;

    out << "Result" << std::endl;
    out << "Débit " << QString::number(dosimetry.GetDebitMean(), 'f', 4).toStdString() + " +- " + QString::number(dosimetry.GetDebitStdDev(), 'f', 4).toStdString() << " (cGy/UM)" << std::endl;
    out << std::endl;

    std::list<DosimetryRecord> records = dosimetry.GetRecords();
    out << "Measurements" << std::endl;
    if (!records.empty()) {
           out << "Duration\tUM_prev\tUM_del\tcharge" << std::endl;
        for (DosimetryRecord record : records) {
            out << record;
        }
    } else {
        out << "None" << std::endl;
    }
    return out;
}

bool operator==(const Dosimetry &d1, const Dosimetry& d2) {
    return (d1.GetTimestamp() == d2.GetTimestamp() &&
            d1.IsReference() == d2.IsReference() &&
            d1.IsMonteCarlo() == d2.IsMonteCarlo() &&
            d1.GetTemperature() == d2.GetTemperature() &&
            d1.GetPressure() == d2.GetPressure() &&
            d1.GetChambre() == d2.GetChambre() &&
            d1.GetRecords() == d2.GetRecords());
}

bool operator!=(const Dosimetry &d1, const Dosimetry& d2) {
    return !(d1 == d2);
}




