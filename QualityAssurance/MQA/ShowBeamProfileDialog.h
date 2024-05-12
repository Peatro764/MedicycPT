#ifndef QUALITYASSURANCE_SHOWBEAMPROFILEDIALOG_H
#define QUALITYASSURANCE_SHOWBEAMPROFILEDIALOG_H

#include <QDialog>
#include <memory>

#include "BeamProfile.h"
#include "LinePlot.h"

namespace Ui {
class ShowBeamProfileDialog;
}

class ShowBeamProfileDialog : public QDialog
{
    Q_OBJECT

public:
    ShowBeamProfileDialog(QWidget *parent, BeamProfile profile);
    ~ShowBeamProfileDialog();

private slots:
    void ExportToCSV();
    void Print();

private:
    Ui::ShowBeamProfileDialog *ui_;
    std::shared_ptr<LinePlot> profile_graph_;
    BeamProfile profile_;
}
;

#endif

