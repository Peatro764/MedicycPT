
#include "DosimetryRecord.h"

#include <iomanip>

#include "Calc.h"

DosimetryRecord:: DosimetryRecord(const std::vector<double>& duration,
                                  const std::vector<double>& i_chambre1,
                                  const std::vector<double>& i_chambre2,
                                  const double& i_stripper,
                                  const double& i_cf9,
                                  const int& um_prevu,
                                  const std::vector<int>& um_del,
                                  const double& charge,
                                  const double& air_ic_charge) :
    duration_(duration),
    i_chambre1_(i_chambre1), i_chambre2_(i_chambre2),
    i_stripper_(i_stripper), i_cf9_(i_cf9),
    um_prevu_(um_prevu), um_del_(um_del),
    charge_(charge), air_ic_charge_(air_ic_charge) {}

DosimetryRecord:: DosimetryRecord(const int& um_del, const double& charge) :
    duration_(std::vector<double>()),
    i_chambre1_(std::vector<double>()),
    i_chambre2_(std::vector<double>()),
    i_stripper_(0.0),
    i_cf9_(0.0),
    um_prevu_(0),
    um_del_(std::vector<int> {um_del}),
    charge_(charge),
    air_ic_charge_(0.0) {}

DosimetryRecord::~DosimetryRecord() {}

int DosimetryRecord::GetBeamTransmission() const {
    if (std::abs(i_stripper_) > 2) {
            return static_cast<int>(std::round(100.0 * i_cf9_ * 2.0 / i_stripper_));
    } else {
        return 0;
    }
}

double DosimetryRecord::GetTotalDuration() const {
    if (duration_.empty()) {
        return 0.0;
    } else {
        return duration_.back();
    }
}

int DosimetryRecord::GetTotalUMDelivered() const {
    if (um_del_.empty()) {
        return 0;
    } else {
        return um_del_.back();
    }
}

std::ostream& operator<< (std::ostream &out, const DosimetryRecord &record) {
    out << record.GetTotalDuration() << "\t"
        << record.GetUMPrevu() << "\t"
        << record.GetTotalUMDelivered() << "\t"
        << record.GetCharge() << std::endl;

    return out;
}

bool operator==(const DosimetryRecord &record1, const DosimetryRecord& record2) {
    return (calc::AlmostEqual(record1.GetDuration(), record2.GetDuration(), 0.0001) &&
            calc::AlmostEqual(record1.GetIChambre1(), record2.GetIChambre1(), 0.0001) &&
            calc::AlmostEqual(record1.GetIChambre2(), record2.GetIChambre2(), 0.0001) &&
            calc::AlmostEqual(record1.GetIStripper(), record2.GetIStripper(), 0.0001) &&
            calc::AlmostEqual(record1.GetICF9(), record2.GetICF9(), 0.0001) &&
            record1.GetUMPrevu() == record2.GetUMPrevu() &&
            record1.GetUMDelivered() == record2.GetUMDelivered() &&
            calc::AlmostEqual(record1.GetCharge(), record2.GetCharge(), 0.00001) &&
            calc::AlmostEqual(record1.GetAirICCharge(), record2.GetAirICCharge(), 0.00001));
}

bool operator!=(const DosimetryRecord &record1, const DosimetryRecord& record2) {
    return !(record1 == record2);
}

