#ifndef LIBRARYPAGE_H
#define LIBRARYPAGE_H

#include <QObject>
#include <QDialog>
#include <vector>

#include "Modulateur.h"
#include "Degradeur.h"
#include "qcustomplot.h"
#include "SOBPCurve.h"
#include "SOBPMaker.h"
#include "MCNPXData.h"

class MTP;

class LibraryPage : public QObject
{
    Q_OBJECT

public:
     LibraryPage(MTP* parent);
    ~LibraryPage();

public slots:
     void ListModulators();
     void ListDegradeurs();
     void ListDegradeurs(int modulateur_row);
     void ListDegradeurs(double des_parcours);
     void CalculateSOBPCurves();
     void UpdateZHalf();
     void mousePress();
     void mouseWheel();
     void SelectModulateur(int table_row);
     void SelectDegradeur(int table_row);
     void AssociateToDossier();


private:

     enum DEGCOL { DEG_ID, DEG_DEGR, DEG_DELTADEGR };
     enum MODCOL { MOD_ID, MOD_MOD, MOD_PARC, MOD_DEGR };

    void SetupDefaults();
    void SetupLineEditValidators();
    void ConnectSignals();
    void ConfigureTables();
    void ConfigurePlot();
    void CalcRefSOBPCurve(const SOBPMaker& sobp_maker);
    void CalcActSOBPCurve(const SOBPMaker& sobp_maker, const Modulateur& mod, const DegradeurSet& deg);
    Modulateur GetActModulateur();
    DegradeurSet GetActDegradeur();
    void CalcIndCurves();
    void WriteModulateursInTable(const std::vector<Modulateur>& modulateurs);
    void WriteDegradeursInTable(const std::vector<DegradeurSet>& deg_sets, const int& des_um_plexi);
    void SortModulateurs(std::vector<Modulateur>& modulateur, double des_mod100, double des_parcours);
    QCPGraph* AddBPGraph();

    MTP* parent_;

    QCPGraph* ref_sobp_graph_;
    QCPGraph* sobp_graph_;
    std::vector<QCPGraph*> bp_graphs_;
    QCPItemTracer *zhalf_tracer;
    QCPItemTracer *ref_zhalf_tracer;
    SOBPCurve sobp_curve_;
    SOBPCurve ref_sobp_curve_;


};

#endif
