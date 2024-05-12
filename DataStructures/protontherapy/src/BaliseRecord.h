#ifndef PROTONTHERAPIE_BALISERECORD_H
#define PROTONTHERAPIE_BALISERECORD_H

#include <QDateTime>

class BaliseRecord {

public:
    BaliseRecord(const QDateTime& timestamp,
                 int dossier,
                 int seance_record_id,
                 double charge,
                 double ref_debit,
                 double tdj_debit,
                 double patient_debit,
                 double dose_gy);
    ~BaliseRecord();
    QDateTime GetTimestamp() const { return timestamp_; }
    int GetDossierNumber() const { return dossier_; }
    int GetSeanceRecordId() const { return seance_record_id_; }
    double GetCharge() const { return charge_; }
    double GetRefDebit() const { return ref_debit_; }
    double GetTDJDebit() const { return tdj_debit_; }
    double GetPatientDebit() const { return patient_debit_; }
    double GetDose() const { return dose_gy_; }

private:
    QDateTime timestamp_;
    int dossier_;
    int seance_record_id_;
    double charge_;
    double ref_debit_;
    double tdj_debit_;
    double patient_debit_;
    double dose_gy_;
};

 bool operator==(const BaliseRecord &b1, const BaliseRecord& b2);
 bool operator!=(const BaliseRecord &b1, const BaliseRecord& b2);

#endif // PROTONTHERAPIE_BALISERECORD_H
