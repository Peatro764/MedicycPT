#ifndef NUMERICINPUTDIALOG_H
#define NUMERICINPUTDIALOG_H

#include <QDialog>

namespace Ui {
class NumericInputDialog;
}

class NumericInputDialog : public QDialog
{
    Q_OBJECT

public:
    NumericInputDialog(QWidget *parent);

public slots:
    int Dossier() { return dossier_; }
    void AddNumber(int number);

private:
    Ui::NumericInputDialog *ui_;
    int dossier_ = 0;
};

#endif

