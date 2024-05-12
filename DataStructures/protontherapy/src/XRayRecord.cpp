#include "XRayRecord.h"

#include "Calc.h"

XRayRecord::XRayRecord(const QDateTime& timestamp,
                       int dossier,
                       double peak_value,
                       double integrated_charge)
    : timestamp_(timestamp),
      dossier_(dossier),
      peak_value_(peak_value),
      integrated_charge_(integrated_charge) {}

XRayRecord::~XRayRecord() {}

bool operator==(const XRayRecord &b1, const XRayRecord& b2) {
    return (b1.GetTimestamp() == b2.GetTimestamp() &&
            b1.GetDossierNumber() == b2.GetDossierNumber() &&
            calc::AlmostEqual(b1.GetPeakValue(), b2.GetPeakValue(), 0.0001) &&
            calc::AlmostEqual(b1.GetIntegratedCharge(), b2.GetIntegratedCharge(), 0.0001));
}

bool operator!=(const XRayRecord &b1, const XRayRecord& b2) {
    return !(b1 == b2);
}


