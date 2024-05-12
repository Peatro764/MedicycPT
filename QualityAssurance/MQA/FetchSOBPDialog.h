#ifndef QUALITYASSURANCE_FETCHSOBPDIALOG_H
#define QUALITYASSURANCE_FETCHSOBPDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QTimer>
#include <QTreeWidget>
#include <QStringList>

#include "QARepo.h"
#include "SOBP.h"

namespace Ui {
class FetchSOBPDialog;
}

class FetchSOBPDialog : public QDialog
{
    Q_OBJECT
public:
    FetchSOBPDialog(QWidget* parent, std::shared_ptr<QARepo> repo);
    ~FetchSOBPDialog();
public slots:
    SOBP sobp() const { return sobp_; }
    void ItemClicked(QTreeWidgetItem* item, int column);
    void DownloadItem(QTreeWidgetItem* item, int column);

signals:

private:
    void ConnectSignals();
    void FillDirectories();
    void FillData(QTreeWidgetItem *item);
    void AddLeaf(QTreeWidgetItem *parent, QDateTime timestamp, double rs_mm_plexi, int dossier, QString comment);
    void DisplayModelessMessageBox(QString msg, QMessageBox::Icon icon);
    Ui::FetchSOBPDialog *ui_;
    std::shared_ptr<QARepo> repo_;
    SOBP sobp_;
};

#endif // FetchSOBPDialog_H
