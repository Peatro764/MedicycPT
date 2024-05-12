#ifndef PROTONTHERAPIE_SEANCE_H
#define PROTONTHERAPIE_SEANCE_H

#include <list>
//#include <QDateTime>

#include "SeanceRecord.h"

class Seance {
public:
    Seance(const int& seance_id,
           const double& dose_prescribed,
           const std::list<SeanceRecord>& seance_records);
    Seance();
    ~Seance();

    bool Started() const { return !seance_records_.empty(); }
    int GetSeanceId() const { return seance_id_; }
    double GetDosePrescribed() const { return dose_prescribed_; }
    std::list<SeanceRecord> GetSeanceRecords() const { return seance_records_; }
    double GetDoseDelivered() const;
    double GetDoseEstimated() const;
    void AddSeanceRecord(const SeanceRecord& record);

private:
    int seance_id_;
    double dose_prescribed_;
    std::list<SeanceRecord> seance_records_;
};

std::ostream& operator<< (std::ostream &out, const Seance &seance);
bool operator==(const Seance& s1, const Seance& s2);
bool operator!=(const Seance& s1, const Seance& s2);

#endif // PROTONTHERAPIE_SEANCE_H
