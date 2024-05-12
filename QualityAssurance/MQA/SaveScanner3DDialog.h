#ifndef QUALITYASSURANCE_SaveScanner3DDIALOG_H
#define QUALITYASSURANCE_SaveScanner3DDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class SaveScanner3DDialog;
}

class SaveScanner3DDialog : public QDialog
{
    Q_OBJECT

public:
    SaveScanner3DDialog(QWidget *parent);
    QString comment() const { return comment_; }

public slots:

private slots:
    void GetParameters();

private:
    void SetupValidators();
    Ui::SaveScanner3DDialog *ui_;
    QString comment_;
};

#endif

