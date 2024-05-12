#ifndef MillingMachinePROGRAMDIALOG_H
#define MillingMachinePROGRAMDIALOG_H

#include <QDialog>

namespace Ui {
class MillingMachineProgramDialog;
}

class MillingMachineProgramDialog : public QDialog
{
    Q_OBJECT

public:
    MillingMachineProgramDialog(QWidget *parent, QString program);

private slots:
    void SaveToFile();

private:
     Ui::MillingMachineProgramDialog *ui_;
     QString program_;

};

#endif // MillingMachinePROGRAMDIALOG_H
