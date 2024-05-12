#ifndef PROGRAMTYPEDIALOG_H
#define PROGRAMTYPEDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QTimer>

#include "MillingMachine.h"

namespace Ui {
class ProgramTypeDialog;
}

class ProgramTypeDialog : public QDialog
{
    Q_OBJECT
public:
    ProgramTypeDialog(QWidget* parent, int dossier, QString program_type);
    ~ProgramTypeDialog();
public slots:
    int Id() const ;
    QString ProgramType() const;
    void UpdateIdLabel();

signals:

private:
    void LoadProgramTypes();
    Ui::ProgramTypeDialog *ui_;
    int id_;
    QString program_type_;
};

#endif
