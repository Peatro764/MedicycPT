#ifndef QUALITYASSURANCE_SeanceParameterSelectorDialog_H
#define QUALITYASSURANCE_SeanceParameterSelectorDialog_H

#include <QDialog>
#include <QDateTime>

#include "SeanceConfig.h"

namespace Ui {
class SeanceParameterSelectorDialog;
}

class SeanceParameterSelectorDialog : public QDialog
{
    Q_OBJECT

public:
    SeanceParameterSelectorDialog(QWidget *parent, SeanceConfig config);
    ~SeanceParameterSelectorDialog() {}
    SeanceConfig GetSeanceConfig() const { return config_; }

private slots:

private:
    void SetupSpinboxes();
    void ConnectSignals();
    void SetInitialValues();
    void SetUM(int um);
    void SetDuration(int ms);
    void SetStripperCurrent(int nA);
    void SetupLimits();
    int s2ms(double s) const;
    double ms2s(int ms) const;


    Ui::SeanceParameterSelectorDialog *ui_;
    SeanceConfig config_;

    const int max_duration_ = 60000;
    const int min_duration_ = 1000;
    const int max_stripper_current_ = 1000;
    const int min_stripper_current_ = 1;
    const int max_um_ = 4000;
    const int min_um_ = 1;

};

#endif

