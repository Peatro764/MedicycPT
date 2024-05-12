#ifndef COMPENSATEURTRANSPARANTDIALOG_H
#define COMPENSATEURTRANSPARANTDIALOG_H

#include <QDialog>

#include "Compensateur.h"
#include "Patient.h"
#include "qcustomplot.h"

namespace Ui {
class CompensateurTransparantDialog;
}

class CompensateurTransparantDialog : public QDialog
{
    Q_OBJECT

public:
    CompensateurTransparantDialog(QWidget *parent, const Compensateur& compensateur, const Patient& patient);
    void Print(QPrinter* printer);
    QWidget* Widget();
    QPrinter::Orientation Orientation() { return QPrinter::Portrait; }

public slots:
    void Print();

private:
    void Draw(const Compensateur& compensateur, const Patient& patient);
    QCPCurveDataContainer MakeCircle(const double radius, const QCPCurveData& centre, double offset_x, double offset_y) const;
    void Offset(QCPCurveDataContainer &data, double offset_x, double offset_y);
     Ui::CompensateurTransparantDialog *ui_;

     QCPCurve *compXY_graph_;
     QCPCurve *compXZ_graph_;

     QCPCurve *eyeXZ_graph_;
     QCPCurve *sclereXZ_graph_;

     QCPCurve *list_mmplexi_;

     QCPCurve *isodoseXZ1_graph_;
     QCPCurve *isodoseXZ2_graph_;
     QCPCurve *list_dossier_;

     QCPCurve *compensateur_base_;

};

#endif
