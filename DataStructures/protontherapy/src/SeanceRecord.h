#ifndef PROTONTHERAPIE_SEANCERECORD_H
#define PROTONTHERAPIE_SEANCERECORD_H

#include <QDateTime>
#include <vector>

class SeanceRecord {

public:
    SeanceRecord(const QDateTime& timestamp,
                 const double& debit,
                 const std::vector<double>& duration,
                 const std::vector<double>& i_chambre1,
                 const std::vector<double>& i_chambre2,
                 const std::vector<int>& cf9_status,
                 const double& i_stripper,
                 const double& i_cf9,
                 const int& um_prevu,
                 const int& um_corr,
                 const std::vector<int>& um_del_1,
                 const std::vector<int>& um_del_2,
                 const double& dose_est);
    SeanceRecord(const double& debit, const int& um_del_1);
    ~SeanceRecord();
    QDateTime GetTimestamp() const { return timestamp_; }
    double GetDebit() const { return debit_; }
    std::vector<double> GetDuration() const { return duration_ ;}
    double GetTotalDuration() const;
    std::vector<double> GetIChambre1() const { return i_chambre1_; }
    std::vector<double> GetIChambre2() const { return i_chambre2_; }
    std::vector<int> GetCF9Status() const { return cf9_status_; }
    double GetIStripper() const { return i_stripper_; }
    double GetICF9() const { return i_cf9_; }
    int GetUMPrevu() const { return um_prevu_; }
    int GetUMCorr() const { return um_corr_; }
    std::vector<int> GetUM1Delivered() const { return um_del_1_; }
    std::vector<int> GetUM2Delivered() const { return um_del_2_; }
    int GetTotalUM1Delivered() const;
    int GetTotalUM2Delivered() const;
    int GetTotalUMDelivered() const;
    double GetDoseDelivered() const;
    double GetDoseEstimated() const { return dose_est_; }
    bool PartOf(const SeanceRecord& record) const;
    static SeanceRecord GetRandom();
    SeanceRecord Subtract(const std::vector<SeanceRecord>& records) const;
    SeanceRecord Subtract(int um1, int um2, double duration) const;

    void SetEstimatedDose(double dose_est) { dose_est_ = dose_est; }

private:
    QDateTime timestamp_;
    double debit_;
    std::vector<double> duration_;
    std::vector<double> i_chambre1_;
    std::vector<double> i_chambre2_;
    std::vector<int> cf9_status_;
    double i_stripper_;
    double i_cf9_;
    int um_prevu_;
    int um_corr_;
    std::vector<int> um_del_1_;
    std::vector<int> um_del_2_;
    double dose_est_ = 0.0;
};

 std::ostream& operator<< (std::ostream &out, const SeanceRecord &record);
 bool operator==(const SeanceRecord &s1, const SeanceRecord& s2);
 bool operator!=(const SeanceRecord &s1, const SeanceRecord& s2);

#endif // PROTONTHERAPIE_SEANCERECORD_H
