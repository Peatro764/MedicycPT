#ifndef QA_BEAMLINEGUI_H
#define QA_BEAMLINEGUI_H

#include <QWidget>

#include "GuiBase.h"
#include "PTRepo.h"
#include "TimedStampedDataGraph.h"

#include "Dosimetry.h"
#include "DosimetryRecord.h"


class MTD;

class BeamLineGui : public GuiBase
{
    Q_OBJECT
public:
    BeamLineGui(MQA *parent, std::shared_ptr<PTRepo> repo);
    ~BeamLineGui();

public slots:

    void Configure() override;
    void CleanUp() override;

signals:

private slots:
    void FillTransmissionPageGraphs();

private:
    void ConnectSignals();
    void InitDateSelector();
    void SetupGraphs();

    std::shared_ptr<PTRepo> repo_;
    std::shared_ptr<TimedStampedDataGraph> transmission_graph_;
    QString transmission_graph_name_ = "Transmission Faisceau Stripper -> CF9";
};

#endif
