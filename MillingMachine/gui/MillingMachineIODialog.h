#ifndef MillingMachineIODIALOG_H
#define MillingMachineIODIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QTimer>

#include "DataReader.h"

class MillingMachineIODialog : public QDialog
{
    Q_OBJECT
public:
    MillingMachineIODialog(QWidget* parent, DataReader* data_reader);
    ~MillingMachineIODialog();
public slots:
    void Abort();
    void Error(QString error);
    void DataReady(QString data);
    QString GetData() { return data_; }
    void Read();

signals:

private:
    void StartProgressBar();
    void StopProgressBar();
    QProgressBar* progressBar_;
    QLabel* textLabel_;
    QDialogButtonBox* buttonBox_;
    QPushButton* abortButton_;
    QPushButton* retryButton_;

    DataReader* data_reader_;
    QString data_;
    QTimer timer_;
};

#endif // MillingMachineIODIALOG_H
