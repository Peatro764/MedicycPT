#ifndef QUALITYASSURANCE_Scanner3DGui_H
#define QUALITYASSURANCE_Scanner3DGui_H

#include "GuiBase.h"

#include <QWidget>
#include <QTimer>
#include <map>
#include <memory>

#include "CuveCustomPlot.h"
#include "Scanner3D.h"
#include "DevFactory.h"
#include "Beacon.h"
#include "QARepo.h"
#include "CuveCube.h"
#include "Histogram.h"

class MQA;

class Scanner3DGui : public GuiBase
{
    Q_OBJECT

public:
    Scanner3DGui(MQA* parent, std::shared_ptr<QARepo> repo);
    ~Scanner3DGui();

public slots:
    void Configure();
    void CleanUp();

signals:

private slots:
    void ClearAll();
    void ClearResults();
    void ClearGraphs();
    void StartAcquisition();
    void AbortAcquisition();
    void DisplayCube();
    void HandleMeasurement(MeasurementPoint p);
    void SaveToDb();
    void FetchFromDb();

    void InitProjection(PROJECTION proj);
    void LoadProjection(PROJECTION proj);
    void LoadSlice(PROJECTION proj, int slice);
    void ReadSignalNoise();
    void PrintMouseOver(int h_bin, int v_bin, double h_pos, double v_pos, double value);

private:
    void FillComboBoxes();
    void SetDefaults();
    void SetupValidators();
    void SetupGraphs();
    void ConnectSignals();
    void TurnOffConnectedButton();
    void TurnOnConnectedButton();
    void SetAxisTitles(PROJECTION proj);
    void UpdateGraphs();
    void UpdateNoise(double noise);
    PROJECTION CurrentProjection() const;
    Axis DepthAxis(PROJECTION proj) const;
    std::map<Axis, Range> GetRanges() const;
    void SetRanges(const std::map<Axis, Range> ranges);
    std::map<Axis, double> GetStepSizes() const;
    void SetStepSizes(const std::map<Axis, double> step_sizes);
    std::map<Axis, int> GetNSteps(const std::map<Axis, Range>& ranges,
                                  const std::map<Axis, double>& step_sizes) const;
    std::map<Axis, AxisConfig> GetAxisConfigs(const std::map<Axis, int> n_steps,
                                                            const std::map<Axis, Range> ranges,
                                                            const std::map<Axis, double> step_sizes) const;
    std::map<Axis, Range> GetRanges(const std::map<Axis, AxisConfig>& configs);
    std::map<Axis, double> GetStepSizes(const std::map<Axis, AxisConfig>& configs);
    std::map<Axis, int> GetNbins(const std::map<Axis, AxisConfig>& configs);
    double GetNmbMeasurements() const;
    double GetMaxStdError() const;
    bool GetFixedNmbMeasurements() const;

    std::shared_ptr<Scanner3D> scanner_;
    CuveCube cube_;
    std::shared_ptr<QARepo> repo_;
    std::shared_ptr<CuveCustomPlot> projection_graph_;
    std::shared_ptr<DevFactory> dev_factory_;
    Beacon abort_beacon_;
    std::shared_ptr<Histogram> chambre_histogram_;
    std::shared_ptr<Histogram> diode_histogram_;
};

#endif
