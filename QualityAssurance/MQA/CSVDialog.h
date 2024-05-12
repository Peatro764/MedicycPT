#ifndef QUALITYASSURANCE_CSVDIALOG_H
#define QUALITYASSURANCE_CSVDIALOG_H

#include <QDialog>

#include "QARepo.h"

namespace Ui {
class CSVDialog;
}

class CSVDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CSVDialog(QWidget *parent, std::shared_ptr<QARepo> repo);
    ~CSVDialog();

public slots:
    void ExportSelected();
    void EnableButtons();

private:
    void SetupConnections();
    void Export(QString basename, const BraggPeak& braggpeak) const;
    void Export(QString basename, const SOBP& sobp) const;
    void Export(QString basename, const BeamProfile& beamprofile, Axis axis) const;
    Ui::CSVDialog *ui_;
    std::shared_ptr<QARepo> repo_;
};

#endif
