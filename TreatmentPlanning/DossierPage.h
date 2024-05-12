#ifndef DOSSIERPAGE_H
#define DOSSIERPAGE_H

#include <QObject>
#include <QDialog>
#include <vector>
#include <QTableWidgetItem>

class AlignedQTableWidgetItem : public QTableWidgetItem {
public:
    AlignedQTableWidgetItem(QString text, Qt::Alignment alignment)
        : QTableWidgetItem(text) {
        this->setTextAlignment(alignment);
    }
};

class MTP;

class DossierPage : public QObject
{
    Q_OBJECT

public:
     DossierPage(MTP* parent);
    ~DossierPage();

public slots:
     void GetDossiers();

     void LaunchCreateNewDossierDialog();

     void ShowContextMenu(int row, int col);
     // triggered by actions in context menu
     void ChangeTreatmentType(int row);
     void ModifyPatientName(int row);
     void IncrementActiveSeance(int row);
     void AddSeance(int row);
     void DeleteSeance(int row);
     void ShowDeliveredDose(int row);
     void ModifyDeliveredDose(int row);
     void ModifyPlannedDose(int row);
     void ShowModulateur(int row);
     void ShowModulateurMillingProgram(int row);
     void ShowCollimator(int row);
     void ShowCollimatorMillingProgram(int row);
     void ShowCompensateur(int row);
     void ShowCompensateurMillingProgram(int row);
     void DossierRowClicked(int row);
     void ShowComments(int row);

signals:
     void PrintDossier(int dossier);
     void DbConnectionBroken();

private:
    void SetupLineEditValidators();
    void ConnectSignals();
    void ConfigureTable();
    void SetDefaults();
    void FillComboboxes();
    int GetDossierNumber(int table_row);
    void FillRow(int row, int dossier);

    MTP* parent_;
    int dossier_;
    enum class COLUMNS : int { DOSSIER = 0, PATIENT = 1, TREATMENT_TYPE = 2, MODULATEUR = 3, DEGRADEUR = 4, EPAISSEUR = 5, COLLIMATEUR = 6, COMPENSATEUR = 7, DMESURE = 8,
                               DMCNPX = 9, DDEBROCA = 10, DRATIO = 11, SEANCES = 12, DOSE = 13, XRAYS = 14, COMMENTS = 15 };
};

#endif
