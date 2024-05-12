#ifndef MODULATEURPAGE_H
#define MODULATEURPAGE_H

#include <QObject>
#include <QDialog>
#include <vector>
#include <QColor>

#include "Modulateur.h"
#include "ModulateurMaker.h"
#include "ModulateurGraph.h"

class MTP;

class ModulateurPage : public QObject
{
    Q_OBJECT

public:
     ModulateurPage(MTP* parent);
    ~ModulateurPage();

public slots:
     void CreateModulateur();
     void InitModulateurMaker();
//     void InitModulateurMaker(const std::vector<int> um_plexi_steps);
     void PlotCurves();
     void UpdateAngleColumn();
     void DrawModulateur();
     void SaveModulatorToDb();
     void SaveMillingProgramToDb();
     void FetchFromDatabase();
     double GetMaxRangeMMTissue(QString data_set) const;
     void DegradeurUpdated();

     void ShowFraiseuseProgram();

private:
    void SetDefaults();
    void SetupLineEditValidators();
    void ConnectSignals();
    void ClearData();
    void InitPlot();
    void UpdateTable(int baseplate_um, const um_plexi_weight& new_weights, const um_plexi_weight& old_weights);
    void PlotDepthDoseCurve(/*int um_plexi, */const DepthDoseCurve& curve, const QColor& color);
    void PlotSumCurve(const DepthDoseCurve& curve);
    void PlotDesiredParcours();
    um_plexi_weight SubtractFromKeys(const um_plexi_weight& weights, int value) const;
    Modulateur ConstructModulateurFromData();
    um_plexi_weight GetWeightsFromTable() const;
    um_plexi_depthDoseCurve GetDepthDoseCurves(const std::vector<int>& um_plexi);
    um_plexi_weight GetAnglesFromTable() const;

    enum COL { MMPLEXI, WEIGHT, DWEIGHT, ANGLE };
    enum DATASET { MCNPX, SHIFTEDBRAGG };
    enum OPTALG { JOEL99 };


    MTP* parent_;
    ModulateurMaker modulateur_maker_;
    ModulateurGraph modulateur_graph_;
};

#endif
