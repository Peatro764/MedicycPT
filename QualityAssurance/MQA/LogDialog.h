#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <QDialog>

namespace Ui {
class LogDialog;
}

class LogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogDialog(QWidget *parent, QString path);
    ~LogDialog();

public slots:

private:
    void SetupConnections();
    void Fill(QString path);
    Ui::LogDialog *ui_;
};

#endif
