#ifndef MODULATEURDIALOG_H
#define MODULATEURDIALOG_H

#include <QDialog>

#include "ModulateurGraph.h"
#include "Modulateur.h"
#include "qcustomplot.h"

#include <memory>

namespace Ui {
class ModulateurDialog;
}

class ModulateurDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModulateurDialog(QWidget *parent, const Modulateur& modulateur);
    ~ModulateurDialog();

private:
    Ui::ModulateurDialog *ui_;
    std::unique_ptr<ModulateurGraph> modulateur_graph_;
    Modulateur modulateur_;
};

#endif
