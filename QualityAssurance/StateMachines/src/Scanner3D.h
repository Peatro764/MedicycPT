#ifndef QUALITYASSURANCE_Scanner3D_H
#define QUALITYASSURANCE_Scanner3D_H

#include <QObject>
#include <QStateMachine>
#include <QTimer>
#include <QDebug>

#include "Beacon.h"
#include "DevFactory.h"
#include "Range.h"
#include "IMotor.h"
#include "BeamSignal.h"
#include "AxisScan.h"
#include "AxisStepper.h"
#include "MeasurementPoint.h"

class Scanner3D : public QObject
{
    Q_OBJECT
public:    
    Scanner3D(std::shared_ptr<DevFactory> dev_factory, Beacon* abort_beacon);
    ~Scanner3D();

signals:
    void Measurement(MeasurementPoint p);
    void Finished();
    void AllAxesReady();
    void MoveToStart();
    void StopStateMachine();

public slots:
    void Start(std::map<Axis, Range> ranges,
               std::map<Axis, double> steps,
               int n_measurements, double max_std_err, bool fixed_nmb_measurements);

private slots:
    void StateMachineMessage(QString message) { qDebug() << "Scanner3D " << " " << message; }

private:
    void SetupTimer();
    void ConnectSignals();
    void SetupStateMachine();
    void SetupCube();
    void SetupAxisScanners();
    void AddMeasurement(BeamSignal signal);
    Point CurrentPosition() const;
    void CheckAllAxesReady();

    std::map<Axis, double> axis_position_;
    std::map<Axis, double> dpos_;
    std::shared_ptr<AxisScan> x_scanner_;
    std::shared_ptr<AxisStepper> y_stepper_;
    std::shared_ptr<AxisStepper> z_stepper_;
    std::shared_ptr<DevFactory> dev_factory_;
    Beacon* abort_beacon_;
    QStateMachine sm_;
    QTimer timer_;

    int axes_ready_ = 0;
};


#endif
