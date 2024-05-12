#include "BaliseRecord.h"

#include "Calc.h"

BaliseRecord::BaliseRecord(const QDateTime& timestamp,
                           int dossier,
                           int seance_record_id,
                           double charge,
                           double ref_debit,
                           double tdj_debit,
                           double patient_debit,
                           double dose_gy)
    : timestamp_(timestamp),
      dossier_(dossier),
      seance_record_id_(seance_record_id),
      charge_(charge),
      ref_debit_(ref_debit),
      tdj_debit_(tdj_debit),
      patient_debit_(patient_debit),
      dose_gy_(dose_gy) {}

BaliseRecord::~BaliseRecord() {}

bool operator==(const BaliseRecord &b1, const BaliseRecord& b2) {
    return (b1.GetTimestamp() == b2.GetTimestamp() &&
            b1.GetDossierNumber() == b2.GetDossierNumber() &&
            b1.GetSeanceRecordId() == b2.GetSeanceRecordId() &&
            calc::AlmostEqual(b1.GetCharge(), b2.GetCharge(), 0.0001) &&
            calc::AlmostEqual(b1.GetRefDebit(), b2.GetRefDebit(), 0.0001) &&
            calc::AlmostEqual(b1.GetTDJDebit(), b2.GetTDJDebit(), 0.0001) &&
            calc::AlmostEqual(b1.GetPatientDebit(), b2.GetPatientDebit(), 0.0001) &&
            calc::AlmostEqual(b1.GetDose(), b2.GetDose(), 0.0001));
}

bool operator!=(const BaliseRecord &b1, const BaliseRecord& b2) {
    return !(b1 == b2);
}


