#ifndef PROTONTHERAPIE_XRAYRECORD_H
#define PROTONTHERAPIE_XRAYRECORD_H

#include <QDateTime>

class XRayRecord {

public:
    XRayRecord(const QDateTime& timestamp,
               int dossier,
               double peak_value,
               double integrated_charge);
    ~XRayRecord();
    QDateTime GetTimestamp() const { return timestamp_; }
    int GetDossierNumber() const { return dossier_; }
    double GetPeakValue() const { return peak_value_; }
    double GetIntegratedCharge() const { return integrated_charge_; }

private:
    QDateTime timestamp_;
    int dossier_;
    double peak_value_;
    double integrated_charge_;
};

 bool operator==(const XRayRecord &b1, const XRayRecord& b2);
 bool operator!=(const XRayRecord &b1, const XRayRecord& b2);

#endif
