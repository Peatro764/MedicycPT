#ifndef DepthDoseTypeDIALOG_H
#define DepthDoseTypeDIALOG_H

#include <QDialog>

#include "PTRepo.h"



namespace Ui {
class DepthDoseTypeDialog;
}

class DepthDoseTypeDialog : public QDialog
{
    Q_OBJECT

public:
    DepthDoseTypeDialog(QWidget *parent);

public slots:
    bool BraggPeakChosen() const;
    bool SOBPJourChosen() const;
    bool SOBPPatientChosen() const;
    int modulateur() const { return modulateur_; }
    int dossier() const { return dossier_; }
    double degradeur_mm() const { return degradeur_mm_; }
    QString comment() const { return comment_; }

private slots:
    void ValidateParameters();

private:
    void SetupValidators();
    Ui::DepthDoseTypeDialog *ui_;
    int modulateur_ = 0;
    double degradeur_mm_ = 0.0;
    int dossier_ = 0;
    QString comment_;
};

#endif

