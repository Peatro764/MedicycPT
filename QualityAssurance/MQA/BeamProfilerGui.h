#ifndef QUALITYASSURANCE_BEAMPROFILERGUI_H
#define QUALITYASSURANCE_BEAMPROFILERGUI_H

#include "GuiBase.h"

#include <QWidget>
#include <QTimer>
#include <map>
#include <memory>

#include "BeamProfiler.h"
#include "LinePlot.h"
#include "BeamProfileSeries.h"
#include "DevFactory.h"
#include "Beacon.h"
#include "QARepo.h"
#include "BeamProfilerHistory.h"

class MQA;


class BeamProfilerGui : public GuiBase
{
    Q_OBJECT

public:
    BeamProfilerGui(MQA* parent, std::shared_ptr<QARepo> repo, QA_HARDWARE hardware);
    ~BeamProfilerGui();

public slots:
    void Configure();
    void CleanUp();
    bool IsIdle() const;

signals:
    void StartAcquisitionOnBothAxes();
    void SIGNAL_AxisDeviation(Axis axis, double value);

private slots:
    void ClearAll(Axis axis);
    void ClearResults(Axis axis);
    void ClearGraphs(Axis axis);
    void AddDataPointToContainer(Axis axis, MeasurementPoint m);
    void AddDataPointToGraph(Axis axis, MeasurementPoint m);

    void StartAcquisition(Axis axis);
    void StartAutoCenter(Axis axis);
    void AbortAcquisition();
    void MovingToZero();
    void DisplayBeamProfile(Axis axis);
    void UpdateResults(Axis axis);
    void UpdateGraphs(Axis axis);
    Range GetRange(Axis axis) const;
    double GetStep(Axis axis) const;
    double GetNmbMeasurements() const;
    double GetMaxStdError() const;
    bool GetFixedNmbMeasurements() const;
    void ReadSignalNoise();

    void LowerScanRangeChanged(Axis axis, double value);
    void UpperScanRangeChanged(Axis axis, double value);

    void SaveToDb();
    void TabWidgetPageChanged(int index);
    void LaunchManualMotorControlDialog(Axis axis, std::shared_ptr<IMotor> motor);
    void AutoCenter(Axis axis);
    void Normalize();
    void ToggleVisibleButton(Axis axis);

private:
    void SetDefaults();
    void SetRanges();
    void SetupGraphs();
    void SetupBeamProfiles();
    void SetupTimer();
    void SetupStateMachine();
    void ConnectSignals();
    void TurnOffConnectedButton();
    void TurnOnConnectedButton();
    double GetZeroOffset(Axis axis) const;
    void StateMachineMessage(QString message) { qDebug() << "BeamProfilerGui " << " " << message; }
    bool VerifyAxesCalibrations();
    void VerifyAxisCalibration(Axis axis);

    std::shared_ptr<DevFactory> dev_factory_;
    std::map<Axis, std::shared_ptr<BeamProfile>> beam_profiles_;
    std::shared_ptr<BeamProfiler> profiler_sm_;
    std::shared_ptr<QARepo> repo_;
    BeamProfilerHistory history_page_;
    QStateMachine sm_;
    QTimer timer_;
    Beacon abort_beacon_;
    std::shared_ptr<LinePlot> profile_graph_;
    enum class TABWIDGETPAGE : int { ACQUISITION = 0, RESULTS = 1 };

    QA_HARDWARE hardware_ = QA_HARDWARE::UNK;
    int graph_update_counter_ = 0;
    bool signal_noise_read_once_ = false;
    bool auto_center_ = false;
};

#endif
