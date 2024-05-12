#ifndef SAVEPROGRAMDIALOG_H
#define SAVEPROGRAMDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QTimer>
#include <QTreeWidget>
#include <QStringList>

#include "PTRepo.h"
#include "MillingMachine.h"

namespace Ui {
class SaveProgramDialog;
}

class SaveProgramDialog : public QDialog
{
    Q_OBJECT
public:
    SaveProgramDialog(QWidget* parent, PTRepo *repo, QString program_id, QString program_type);
    ~SaveProgramDialog();
public slots:
    void ItemClicked(QTreeWidgetItem* item, int column);
    void SaveProgram();
    QString ProgramType() const { return program_type_; }
    QString ProgramId() const  { return program_id_; }

signals:

private:
    void ConnectSignals();
    void SetCurrentDirectory(QString program_type);
    void FillProgramTypes();
    void FillMiscDirectories();
    Ui::SaveProgramDialog *ui_;
    PTRepo *repo_;
    QStringList misc_types_;
    QString program_id_;
    QString program_type_;
};

#endif // SAVEPROGRAMDIALOG_H
