#ifndef CREATEDOSSIERDIALOG_H
#define CREATEDOSSIERDIALOG_H

#include <QDialog>

#include "PTRepo.h"
#include "TreatmentType.h"
#include <map>

namespace Ui {
class CreateDossierDialog;
}

class CreateDossierDialog : public QDialog
{
    Q_OBJECT

public:
    CreateDossierDialog(QWidget *parent, PTRepo *repo);
    ~CreateDossierDialog();

public slots:
    void CreateDossier();
    int CreateNewPatient(QString first_name, QString last_name); // can throw
    //void SelectExistingPatient();

private slots:
    void FetchTreatmentTypesData();
    void AddSeance();
    void AddBoost();

private:
    void SetupLineEditValidators();
    void ConnectSignals();

    void InitTreatmentTypesTable();
    void FillTreatmentTypesTable(const std::map<QString, TreatmentType>& types);
    QString GetSelectedTreatmentType() const;
    int GetTreatmentTypeRow(QString type) const;

    std::map<QString, TreatmentType> types_;

    Ui::CreateDossierDialog *ui_;
    PTRepo *repo_;
};

#endif

