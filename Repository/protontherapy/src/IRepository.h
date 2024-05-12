#ifndef PROTONTHERAPIE_IREPOSITORY_H
#define PROTONTHERAPIE_IREPOSITORY_H

#include "Dosimetry.h"
#include "Patient.h"


namespace protontherapie {

class IRepository {
public:
    virtual ~IRepository() {;}
    virtual bool DossierExist(const int& dossier) const = 0;

    virtual bool PatientExist(const int& patient_id) const = 0;
    virtual Patient GetPatient(const int& patient_id) const = 0;
    virtual int SavePatient(const Patient& patient) = 0;

    virtual Dosimetry GetPatientDosimetry(const int& dossier) const = 0;
    virtual Dosimetry GetReferenceDosimetry(const QDateTime& timestamp) const = 0;
    virtual Dosimetry GetTopDeJourDosimetry(const QDate& date) const = 0;
    virtual void SaveDosimetry(const int& dossier, const Dosimetry& dosimetry) = 0;

    /*
    bool DossierExist() const = 0;

    Treatment GetTreatment(const int& dossier) const = 0;
    bool SaveTreatment(const int& dossier, const Treatment& treatment) = 0;

    Dosimetry GetDosimetry(const int& dossier) const = 0;
    bool SaveDosimetry(const int& dossier, const Dosimetry& dosimetry) = 0;
*/
private:
//    IRepository();
//    IRepository(const IRepository&);
//    IRepository& operator =(const IRepository& other);

//    static IRepository instance_;
};

}

#endif // PROTONTHERAPIE_IREPOSITORY_H
