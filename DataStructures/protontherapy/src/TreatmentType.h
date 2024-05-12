#ifndef PROTONTHERAPIE_TREATMENTTYPE_H
#define PROTONTHERAPIE_TREATMENTTYPE_H

#include <list>
#include <QString>
#include <vector>

class TreatmentType {
public:
    TreatmentType(QString name,
                  std::vector<double> dose_seance_std,
                  std::vector<double> dose_seance_boost);
    ~TreatmentType();

    QString Name() const { return name_; }
    std::vector<double> SeanceDoses() const;
    double StandardDose() const;
    double BoostDose() const;
    double TotalDose() const;
    double StandardDoseEBR() const;
    double BoostDoseEBR() const;
    double TotalDoseEBR() const;
    QString StandardSummary() const;
    QString BoostSummary() const;
    QString TotalSummary() const;

    void AddStandard(double dose);
    void AddBoost(double dose);

private:
    double toEBR(double) const;
    QString VectorToString(std::vector<double> v) const;
    QString name_;
    std::vector<double> dose_seance_std_;
    std::vector<double> dose_seance_boost_;
};

bool operator==(const TreatmentType& t1, const TreatmentType& t2);
bool operator!=(const TreatmentType& t1, const TreatmentType& t2);

#endif
