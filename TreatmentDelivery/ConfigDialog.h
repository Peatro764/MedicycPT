#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialogButtonBox>
#include <QLineEdit>
#include <QDialog>
#include <QPushButton>
#include <QMap>
#include <QString>
#include <QSettings>

class QLineEdit;
class QPushButton;

class ConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ConfigDialog(QWidget *parent = 0);

signals:
    
public slots:
    void Save();
    void Cancel();

private:
    QSettings *settings_;
    QDialogButtonBox *buttonBox_;
    QMap<QString, QLineEdit*> lineEdits_;

};

#endif //CONFIGDIALOG_H

