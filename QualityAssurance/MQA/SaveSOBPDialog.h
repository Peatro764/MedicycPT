#ifndef QUALITYASSURANCE_SAVESOBPDIALOG_H
#define QUALITYASSURANCE_SAVESOBPDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class SaveSOBPDialog;
}

class SaveSOBPDialog : public QDialog
{
    Q_OBJECT

public:
    SaveSOBPDialog(QWidget *parent);
    int modulateur() const { return modulateur_; }
    double degradeur_mm() const { return degradeur_mm_; }
    QString comment() const { return comment_; }

public slots:

private slots:
    void GetParameters();

private:
    void SetupValidators();
    Ui::SaveSOBPDialog *ui_;
    int modulateur_ = 0;
    double degradeur_mm_ = 0.0;
    QString comment_;
};

#endif

