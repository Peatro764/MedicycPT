#ifndef PROTONTHERAPIE_DOSIMETRY_H
#define PROTONTHERAPIE_DOSIMETRY_H

#include <list>
#include <QDateTime>
#include <string>

#include "Chambre.h"
#include "Debit.h"
#include "DosimetryRecord.h"

    class Dosimetry {
    public:
        // Constructor used by db when recreating object
        Dosimetry(const QDateTime& timestamp, const bool& reference, const bool& monte_carlo,
                  const double& temperature, const double& pressure, const Chambre& chambre,
                  const std::list<DosimetryRecord>& records, const QString& comment);
        Dosimetry();
        ~Dosimetry();

        void HasBeenSaved();
        bool UnSavedChanges() const;
        bool HasMeasurements() const { return !records_.empty(); }
        QDateTime GetTimestamp() const { return timestamp_; }
        bool IsReference() const { return reference_; }
        bool IsMonteCarlo() const { return monte_carlo_; }
        double GetTemperature() const { return temperature_; }
        double GetPressure() const { return pressure_; }
        Chambre GetChambre() const { return chambre_; }
        std::list<DosimetryRecord> GetRecords() const { return records_; }
        QString GetComment() const { return comment_; }

        void SetTimestamp(const QDateTime& timestamp);
        void SetReference(const bool& reference);
        void SetMonteCarlo(const bool& monte_carlo);
        void SetTemperature(const double& temperature);
        void SetPressure(const double& pressure);
        void SetChambre(const Chambre& chambre);
        void SetComment(const QString& comment);

        bool TemperatureInRange(double n_stddev) const;
        bool PressureInRange(double n_stddev) const;

        Debit GetDebit() const;
        double GetDebitMean() const;
        double GetDebitStdDev() const;
        std::vector<double> GetRollingDebitMeans() const;
        std::vector<double> GetRollingDebitStdDevs() const;
        std::vector<double> GetDebits() const;
        double GetFtp() const;
        double GetExpectedCharge(const double& debit, const int& um) const;

        void AddRecord(const DosimetryRecord& record);
        void DeleteRecords() { records_.clear(); }
        void DeleteSingleRecord(int index);
        bool IsValid() const { return timestamp_.isValid(); }
        QString ToPlainText() const;

        bool TemperatureEntered() const { return temperature_entered_;}
        bool PressureEntered() const { return pressure_entered_; }
        bool ChambreEntered() const { return chambre_entered_; }
        bool MeasurementConditionsEntered() const;

    private:
        QDateTime timestamp_;
        bool reference_;
        bool monte_carlo_;
        double temperature_ = 0.0;
        double pressure_ = 0.0;
        Chambre chambre_;
        QString comment_;

        bool temperature_entered_ = false;
        bool pressure_entered_ = false;
        bool chambre_entered_ = false;

        std::list<DosimetryRecord> records_;
        bool unsaved_changes_;

        double CalcCalibrationConstant(const double& charge, const int& um) const;
    };

     std::ostream& operator<< (std::ostream &out, const Dosimetry &dosimetry);
     bool operator==(const Dosimetry &d1, const Dosimetry& d2);
     bool operator!=(const Dosimetry &d1, const Dosimetry& d2);


#endif
