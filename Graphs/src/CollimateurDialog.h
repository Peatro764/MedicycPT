#ifndef COLLIMATEURDIALOG_H
#define COLLIMATEURDIALOG_H

#include <QDialog>

#include "Collimator.h"

namespace Ui {
class CollimateurDialog;
}

class CollimateurDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CollimateurDialog(const Collimator& collimator, QWidget *parent = 0);
    ~CollimateurDialog();

private:
    Ui::CollimateurDialog *ui;
};

#endif // COLLIMATEURDIALOG_H
