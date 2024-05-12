#ifndef PROTONTHERAPIE_TREATMENT_H
#define PROTONTHERAPIE_TREATMENT_H

#include <vector>

#include "Dosimetry.h"
#include "Patient.h"
#include "Seance.h"
#include "Modulateur.h"
#include "Degradeur.h"
#include "SeanceConfig.h"

class Treatment {
public:
    // Reconstructing an already existing treatment from db
   Treatment(const int& dossier, const Patient& patient,
             const int& active_seance_number,
             const std::vector<Seance>& seances,
             const Dosimetry& patientDosimetry,
             const Dosimetry& patientRefDosimetry,
             const Dosimetry& refDosimetryToday,
             const double& dref,
             const Modulateur& modulateur,
             const DegradeurSet& degradeur_set,
             QString treatment_type,
             QString comments);

   Treatment(const int& dossier,
             const Patient& patient,
             const std::vector<Seance>& seances);
   Treatment();
   ~Treatment();

   SeanceConfig GetSeanceConfig();
   bool IsLastSeance() const;
   double dref() const { return dref_; }
   int GetDossier() const { return dossier_; }
   Patient GetPatient() const { return patient_; }
   int GetActiveSeanceNumber() const { return active_seance_number_; }
   int GetNmbRecords() const;
   Seance* GetActiveSeance();
   Seance GetSeance(int index) const;
   std::vector<Seance> GetAllSeances() { return seances_; }
   int GetNmbSeances() const { return static_cast<int>(seances_.size()); }
   std::vector<Seance> GetPastSeances();
   std::vector<Seance> GetFutureSeances();
   double GetDeliveredDose();
   double GetEstimatedDeliveredDose(); // dose reconstruction using MRM
   double GetPrescribedDose();
   double GetRemainingDoseDossier();
   double GetRemainingDoseActiveSeance();
   double GetDesiredDoseActiveSeance(); // note the difference between desired and planned dose. Desired dose takes into account differnences in previous seances
   int GetUMPrevuActiveSeance();
   void IncrementSeance();
   void Close();
   Dosimetry GetPatientDosimetry() { return patientDosimetry_; }
   Dosimetry GetPatientRefDosimetry() { return patientRefDosimetry_; }
   Dosimetry GetRefDosimetryToday() { return refDosimetryToday_; }
   double GetBeamCalibration() const;
   double GetBeamCalibrationStdDev() const;
   bool BeamCalibrationDone() const;
   Modulateur GetModulateur() const { return modulateur_; }
   DegradeurSet GetDegradeurSet() const { return degradeur_set_; }
   QString ToPlainText();
   QString TreatmentType() const { return treatment_type_; }
   QString Comments() const { return comments_; }
   bool IsFinished() const;

private:
   int dossier_;
   Patient patient_;
   int active_seance_number_;
   std::vector<Seance> seances_;
   Dosimetry patientDosimetry_;
   Dosimetry patientRefDosimetry_;
   Dosimetry refDosimetryToday_;
   double dref_;
   Modulateur modulateur_;
   DegradeurSet degradeur_set_;
   QString treatment_type_;
   QString comments_;
};

#endif // PROTONTHERAPIE_TREATMENT_H
