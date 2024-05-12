#ifndef QUALITYASSURANCE_FetchScanner3DDIALOG_H
#define QUALITYASSURANCE_FetchScanner3DDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QTimer>
#include <QTreeWidget>
#include <QStringList>

#include "QARepo.h"
#include "CuveCube.h"

namespace Ui {
class FetchScanner3DDialog;
}

class FetchScanner3DDialog : public QDialog
{
    Q_OBJECT
public:
    FetchScanner3DDialog(QWidget* parent, std::shared_ptr<QARepo> repo);
    ~FetchScanner3DDialog();
public slots:
    CuveCube cube() const { return cube_; }
    void ItemClicked(QTreeWidgetItem* item, int column);
    void DownloadItem(QTreeWidgetItem* item, int column);

signals:

private:
    void ConnectSignals();
    void FillDirectories();
    void FillData(QTreeWidgetItem *item);
    void AddLeaf(QTreeWidgetItem *parent, QDateTime timestamp, QString comment);
    void DisplayModelessMessageBox(QString msg, QMessageBox::Icon icon);
    Ui::FetchScanner3DDialog *ui_;
    std::shared_ptr<QARepo> repo_;
    CuveCube cube_;
};

#endif // FetchScanner3DDialog_H
