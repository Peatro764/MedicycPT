#ifndef COMPENSATEURPAGE_H
#define COMPENSATEURPAGE_H

#include <QObject>
#include <QDialog>
#include <vector>

#include "Compensateur.h"
#include "Eye.h"
#include "CompXZGraph.h"
#include "CompXYGraph.h"
#include "EyeXZGraph.h"
#include "CompErrorGraph.h"
#include "Patient.h"


class MTP;

class CompensateurPage : public QObject
{
    Q_OBJECT

public:
     CompensateurPage(MTP* parent);
    ~CompensateurPage();

public slots:
     void LoadDossier();
     void CreateAndDraw();
     void SaveToDatabase();
     void FetchFromDatabase();
     void ShowFraiseuseProgram();
     void PrintTransparancy();

private:
    void SetupDefaults();
    void ClearParameters();
    void SetupLineEditValidators();
    void ConnectSignals();
    void SetGraphicsEffects();
    void WritePatient(const Patient& patient);

    void WriteCompensateurData(const Compensateur& compensateur);
    void WriteEyeData(const Eye& eye);
    Eye CreateEye() const;
    Compensateur CreateCompensateur() const;
    void Draw(const Compensateur& compensateur);

    MTP* parent_;
    QSettings settings_;
    CompXYGraph compXY_graph_;
    CompXZGraph compXZ_graph_;
    EyeXZGraph eyeXZ_graph_;
    CompErrorGraph compError_graph_;
    const double base_thickness_ = 1.0;
};

#endif
