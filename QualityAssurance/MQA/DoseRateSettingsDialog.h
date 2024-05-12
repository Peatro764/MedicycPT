#ifndef DOSERATESETTINGSDIALOG_H
#define DOSERATESETTINGSDIALOG_H

#include <QDialog>

#include "Chambre.h"

namespace Ui {
class DoseRateSettingsDialog;
}

class DoseRateSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DoseRateSettingsDialog(const Chambre& chambre, QWidget *parent = 0);
    ~DoseRateSettingsDialog();

public slots:

private:
    void WriteAll();
    Chambre chambre_;
    Ui::DoseRateSettingsDialog *ui;
};

#endif
