#ifndef QUALITYASSURANCE_BRAGGPEAKGUI_H
#define QUALITYASSURANCE_BRAGGPEAKGUI_H

#include <QWidget>
#include <QTimer>
#include <map>
#include <memory>

#include "LinePlot.h"
#include "Histogram.h"
#include "DevFactory.h"
#include "Beacon.h"
#include "QARepo.h"
#include "BraggPeaker.h"
#include "DepthDoseMeasurement.h"
#include "DepthDoseResults.h"
#include "BraggPeakHistory.h"
#include "SOBPHistory.h"
#include "GuiBase.h"

class MQA;

class BraggPeakGui : public GuiBase
{
    Q_OBJECT

public:
    BraggPeakGui(MQA* parent, std::shared_ptr<QARepo> repo, QA_HARDWARE hardware);
    ~BraggPeakGui();

public slots:
    void Configure();
    void CleanUp();
    bool IsIdle() const;

signals:
    void SIGNAL_AxisDeviation(Axis axis, double value);

private slots:
    void ClearAll();
    void ClearResults();
    void ClearGraphs();

    void AddDataPointToContainer(MeasurementPoint m);
    void AddDataPointToGraph(MeasurementPoint m);

    void StartAcquisition();
    void AbortAcquisition();
    void MovingToZero();
    void DisplayBraggPeak();
    void UpdateResults(QString name, const DepthDoseMeasurement& depth_dose);
    void UpdateGraphs();
    Range GetRange() const;
    double GetStep() const;
    double GetNmbMeasurements() const;
    double GetMaxStdError() const;
    bool GetFixedNmbMeasurements() const;
    void ReadSignalNoise();
    void TabWidgetPageChanged(int index);

    void LowerScanRangeChanged(double value);
    void UpperScanRangeChanged(double value);

    void SaveToDb();
    void LaunchManualMotorControlDialog(Axis axis, std::shared_ptr<IMotor> motor);

    void UploadSOBP();
    void ClearUploadedSOBP();
    void Normalize();
    void UpdateResultsPlot();

    void GetSOBPs();

private:
    void SetDefaults();
    void SetRanges();
    void SetupValidators();
    void SetupGraphs();
    void SetupBraggPeak();
    void SetupTimer();
    void ConnectSignals();
    void TurnOffConnectedButton();
    void TurnOnConnectedButton();
    double GetZeroOffset() const;
    void ShowResultIndicators(const DepthDoseResults& results, QString curve);
    bool VerifyAxesCalibrations();
    void VerifyAxisCalibration(Axis axis);

    std::shared_ptr<DevFactory> dev_factory_;
    std::shared_ptr<BraggPeaker> braggpeaker_sm_;
    std::shared_ptr<QARepo> repo_;
    BraggPeakHistory bp_history_page_;
    SOBPHistory sobp_history_page_;
    QTimer timer_;
    Beacon abort_beacon_;
    std::shared_ptr<LinePlot> braggPeak_graph_;
    enum class TABWIDGETPAGE : int { ACQUISITION = 0, BP_RESULTS = 1, SOBP_RESULTS = 2 };
    std::shared_ptr<DepthDoseMeasurement> depth_dose_;

    QString measured_graph_name_ = "Measured";
    QVector<QString> uploaded_graph_names_;

    QA_HARDWARE hardware_ = QA_HARDWARE::UNK;
    int graph_update_counter_ = 0;
    bool signal_noise_read_once_ = false;
};

#endif
