#ifndef PROTONTHERAPIE_DOSIMETRYRECORD_H
#define PROTONTHERAPIE_DOSIMETRYRECORD_H

#include <iostream>
#include <math.h>
#include <vector>

class DosimetryRecord {
public:
    DosimetryRecord(const std::vector<double>& duration,
                    const std::vector<double>& i_chambre1,
                    const std::vector<double>& i_chambre2,
                    const double& i_stripper,
                    const double& i_cf9,
                    const int& um_prevu,
                    const std::vector<int>& um_del,
                    const double& charge,
                    const double& air_ic_charge); // charge measured by the air ionization chamber
    DosimetryRecord(const int& um_del, const double& charge);
    ~DosimetryRecord();

    std::vector<double> GetDuration() const { return duration_ ;}
    double GetTotalDuration() const;
    std::vector<double> GetIChambre1() const { return i_chambre1_; }
    std::vector<double> GetIChambre2() const { return i_chambre2_; }
    double GetIStripper() const { return i_stripper_; }
    double GetICF9() const { return i_cf9_; }
    int GetUMPrevu() const { return um_prevu_; }
    int GetTotalUMDelivered() const;
    std::vector<int> GetUMDelivered() const { return um_del_; }
    double GetCharge() const { return charge_; }
    double GetAirICCharge() const { return air_ic_charge_; }
    int GetBeamTransmission() const;

private:
    std::vector<double> duration_;
    std::vector<double> i_chambre1_;
    std::vector<double> i_chambre2_;
    double i_stripper_;
    double i_cf9_;
    int um_prevu_;
    std::vector<int> um_del_;
    double charge_; // li (nC)
    double air_ic_charge_; // pC
};

 std::ostream& operator<< (std::ostream &out, const DosimetryRecord &record);
 bool operator==(const DosimetryRecord &record1, const DosimetryRecord& record2);
 bool operator!=(const DosimetryRecord &record1, const DosimetryRecord& record2);


#endif
