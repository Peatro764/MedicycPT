#ifndef QUALITYASSURANCE_SOBPRESULTSDIALOG_H
#define QUALITYASSURANCE_SOBPRESULTSDIALOG_H

#include <QDialog>
#include <QString>

#include "SOBP.h"

namespace Ui {
class SOBPResultsDialog;
}

class SOBPResultsDialog : public QDialog
{
    Q_OBJECT

public:
    SOBPResultsDialog(QWidget *parent, const SOBP& sobp);

public slots:

private slots:

private:
    void Fill();
    Ui::SOBPResultsDialog *ui_;
    SOBP sobp_;
};

#endif

