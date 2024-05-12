#ifndef FETCHPROGRAMDIALOG_H
#define FETCHPROGRAMDIALOG_H

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
class FetchProgramDialog;
}

class FetchProgramDialog : public QDialog
{
    Q_OBJECT
public:
    FetchProgramDialog(QWidget* parent, PTRepo *repo);
    ~FetchProgramDialog();
public slots:
    QString Id() const { return id_; }
    QString ProgramType() const { return program_type_; }
    QString Program() const { return program_; }
    QDateTime Timestamp() const { return timestamp_; }
    void ItemClicked(QTreeWidgetItem* item, int column);
    void DownloadItem(QTreeWidgetItem* item, int column);


signals:

private:
    void ConnectSignals();
    void FillProgramTypes();
    void FillMiscDirectories(QTreeWidgetItem *parent);
    void FillMiscData(QTreeWidgetItem *item);
    void FillTreatmentDirectories();
    void FillTreatmentData(QTreeWidgetItem *item);
    void AddLeaf(QTreeWidgetItem *parent, QString name, QDateTime timestamp);
    Ui::FetchProgramDialog *ui_;
    PTRepo *repo_;
    QStringList misc_types_;
    QStringList treatment_types_;
    QString id_;
    QString program_type_;
    QDateTime timestamp_;
    QString program_;
};

#endif // FETCHPROGRAMDIALOG_H
