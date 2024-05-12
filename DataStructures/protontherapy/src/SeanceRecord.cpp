#include "SeanceRecord.h"

#include <stdexcept>
#include <iostream>

#include "Calc.h"

SeanceRecord::SeanceRecord(const QDateTime& timestamp,
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
                            const double& dose_est)
    : timestamp_(timestamp),
      debit_(debit),
      duration_(duration),
      i_chambre1_(i_chambre1),
      i_chambre2_(i_chambre2),
      cf9_status_(cf9_status),
      i_stripper_(i_stripper),
      i_cf9_(i_cf9),
      um_prevu_(um_prevu),
      um_corr_(um_corr),
      um_del_1_(um_del_1),
      um_del_2_(um_del_2),
      dose_est_(dose_est) {}

SeanceRecord::SeanceRecord(const double& debit, const int& um_del_1)
    : timestamp_(QDateTime::currentDateTime()),
      debit_(debit),
      duration_(std::vector<double>()),
      i_chambre1_(std::vector<double>()),
      i_chambre2_(std::vector<double>()),
      cf9_status_(std::vector<int>()),
      i_stripper_(0.0f),
      i_cf9_(0.0f),
      um_prevu_(0),
      um_corr_(0) {
    um_del_1_.clear();
    um_del_2_.clear();
    um_del_1_.push_back(um_del_1);
}


SeanceRecord::~SeanceRecord() {}

double SeanceRecord::GetTotalDuration() const {
    if (duration_.empty()) {
        return 0.0;
    } else {
        return (duration_.back() - duration_.front());
    }
}

int SeanceRecord::GetTotalUMDelivered() const {
    return GetTotalUM1Delivered();
}

int SeanceRecord::GetTotalUM1Delivered() const {
    if (um_del_1_.empty()) {
        return 0;
    } else {
        return um_del_1_.back();
    }
}

int SeanceRecord::GetTotalUM2Delivered() const {
    if (um_del_2_.empty()) {
        return 0;
    } else {
        return um_del_2_.back();
    }
}

double SeanceRecord::GetDoseDelivered() const {
    return calc::UMToDose(debit_, GetTotalUMDelivered());
}

SeanceRecord SeanceRecord::Subtract(const std::vector<SeanceRecord>& records) const {
    (void)records;
    return *this;
}

SeanceRecord SeanceRecord::Subtract(int um1, int um2, double duration) const {
    auto um_del_1_vec = this->GetUM1Delivered();
    auto um_del_2_vec = this->GetUM2Delivered();
    auto duration_vec = this->GetDuration();

    for (auto &um : um_del_1_vec) {
        if (um1 > um) { throw std::runtime_error("UM ne peut pas etre negatif"); }
        um = um - um1;
    }
    for (auto &um : um_del_2_vec) {
        if (um2 > um) { throw std::runtime_error("UM ne peut pas etre negatif"); }
        um = um - um2;
    }
    for (auto &dur : duration_vec) {
        if (duration > dur) { throw std::runtime_error("Duré ne peut pas etre negatif"); }
        dur = dur - duration;
    }

    SeanceRecord sub(this->GetTimestamp(),
                     this->GetDebit(),
                     duration_vec,
                     this->GetIChambre1(), this->GetIChambre2(),
                     this->GetCF9Status(),
                     this->GetIStripper(), this->GetICF9(),
                     this->GetUMPrevu(), this->GetUMCorr(),
                     um_del_1_vec, um_del_2_vec,
                     this->GetDoseEstimated());

    return sub;
}

// Checks if this is a subset of the given SeanceRecord (judging from only a subset of parameters)
bool SeanceRecord::PartOf(const SeanceRecord& record) const {
    return (
        std::equal(duration_.begin(), duration_.end(), record.GetDuration().begin(),
                   [](double a, double b) -> bool { return std::abs(a -b) < 0.001; }) &&
        std::equal(cf9_status_.begin(), cf9_status_.end(), record.GetCF9Status().begin()) &&
        std::equal(um_del_1_.begin(), um_del_1_.end(), record.GetUM1Delivered().begin()) &&
        std::equal(um_del_2_.begin(), um_del_2_.end(), record.GetUM2Delivered().begin()) &&
        std::abs(record.GetDebit() - debit_) < 0.0001 &&
        record.GetUMPrevu() == um_prevu_ &&
        record.GetUMCorr() == um_corr_);
}


std::ostream& operator<< (std::ostream &out, const SeanceRecord &record) {
    out << record.GetTimestamp().toString("dd.MM.yyyy-hh:mm::ss").toStdString() << "\t"
        << record.GetTotalDuration() << "\t"
        << record.GetDebit() << "\t"
        << record.GetUMPrevu() << "\t"
        << record.GetTotalUMDelivered() << "\t"
        << record.GetDoseEstimated() << std::endl;

    return out;
}

SeanceRecord SeanceRecord::GetRandom() {
    const QDateTime time = QDateTime::currentDateTime().addDays(-(rand() % 100)).addSecs(rand() % 100);
    const double debit = static_cast<double>((rand() % 15) + 5) / 10.0;
    const double i_stripper = static_cast<double>((rand() % 1000) + 5);
    const double i_cf9 = static_cast<double>((rand() % 1000) + 5);
    const int um_prevu = static_cast<int>((rand() % 3000) + 1);
    const int um_corr = std::min(um_prevu - 1, rand() % 60 + 1);
    const double dose_est = static_cast<double>((rand() % 145) + 5) / 10.0;

    std::vector<double> duration;
    std::vector<double> i_ch_1;
    std::vector<double> i_ch_2;
    std::vector<int> cf9_status;
    std::vector<int> um_1;
    std::vector<int> um_2;

    duration.push_back(static_cast<double>((rand() % 100) + 1) / 1000.0);
    um_1.push_back(rand() % 30);
    um_2.push_back(rand() % 30);
    i_ch_1.push_back(static_cast<double>((rand() % 1000) + 5));
    i_ch_2.push_back(static_cast<double>((rand() % 1000) + 5));
    cf9_status.push_back(rand() % 1);
    const int n_array_entries = (rand() % 5) + 1;
    for (int idx = 1; idx < n_array_entries; ++idx) {
        duration.push_back(duration.at(idx - 1) + static_cast<double>(rand() % 50 + 50));
        i_ch_1.push_back(i_ch_1.at(idx - 1) + static_cast<double>(rand() % 50) - 25.0);
        i_ch_2.push_back(i_ch_2.at(idx - 1) + static_cast<double>(rand() % 50) - 25.0);
        um_1.push_back(um_1.at(idx - 1) + (rand() % 10 + 10));
        um_2.push_back(um_2.at(idx - 1) + (rand() % 10 + 10));
        cf9_status.push_back(rand() % 1);
    }

    return SeanceRecord(time,
                        debit,
                        duration,
                        i_ch_1,
                        i_ch_2,
                        cf9_status,
                        i_stripper,
                        i_cf9,
                        um_prevu,
                        um_corr,
                        um_1,
                        um_2,
                        dose_est);

}

bool operator==(const SeanceRecord &s1, const SeanceRecord& s2) {
    return (s1.GetTimestamp() == s2.GetTimestamp() &&
            calc::AlmostEqual(s1.GetDebit(), s2.GetDebit(), 0.0001) &&
            calc::AlmostEqual(s1.GetDuration(), s2.GetDuration(), 0.001) &&
            calc::AlmostEqual(s1.GetIChambre1(), s2.GetIChambre1(), 0.001) &&
            calc::AlmostEqual(s1.GetIChambre2(), s2.GetIChambre2(), 0.001) &&
            s1.GetCF9Status() == s2.GetCF9Status() &&
            calc::AlmostEqual(s1.GetIStripper(), s2.GetIStripper(), 0.0001) &&
            calc::AlmostEqual(s1.GetICF9(), s2.GetICF9(), 0.0001) &&
            s1.GetUMPrevu() == s2.GetUMPrevu() &&
            s1.GetUMCorr() == s2.GetUMCorr() &&
            s1.GetUM1Delivered() == s2.GetUM1Delivered() &&
            s1.GetUM2Delivered() == s2.GetUM2Delivered() &&
            calc::AlmostEqual(s1.GetDoseEstimated(), s2.GetDoseEstimated(), 0.001));
}

bool operator!=(const SeanceRecord &s1, const SeanceRecord& s2) {
    return !(s1 == s2);
}


