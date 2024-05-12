#ifndef QUALITYASSURANCE_DEVFACTORY_H
#define QUALITYASSURANCE_DEVFACTORY_H

#include <memory>
#include <map>
#include <queue>
#include <QObject>
#include <QStateMachine>
#include <QTimer>
#include <QDebug>
#include <QSettings>

#include "LabJack.h"
#include "Axis.h"
#include "Clock.h"
#include "IMotor.h"
#include "ISensor.h"
#include "SensorConfig.h"
#include "MotorConfig.h"
#include "Actuator.h"
#include "Range.h"
#include "QARepo.h"

class DevFactory
{
public:
    DevFactory(std::shared_ptr<QARepo> repo);
    ~DevFactory();
    void Setup(QA_HARDWARE hardware); // throws
    void CleanUp();
    bool IsConnected() const;

    std::shared_ptr<IMotor> GetMotor(Axis axis);
    std::shared_ptr<ISensor> GetEncoder(Axis axis);
    std::shared_ptr<ISensor> GetSensor(SensorType type);
    std::shared_ptr<Actuator> GetActuator(ActuatorType type);
    std::vector<double> ReadMultipleSensors(SensorType type1, SensorType type2,
                                            int n_measurements, double max_std_err, bool fixed_nmb_measurements);
    void CalibrateAxisPosition(Axis axis);
    bool VerifyAxisPositionCalibration(Axis axis, double& diff);
private:
    void CreateLabJack();
    void ConnectLabJack();
    void DisconnectLabJack();
    void CreateDevices(QA_HARDWARE hardware);
    void ConfigureDevices();
    Range GetAllowedRange(Axis axis) const;

    void CreateClock(QA_HARDWARE hardware);
    void CreateSensor(SensorType type);
    void CreateEncoder(Axis axis);
    void CreateServoMotor(Axis axis, QA_HARDWARE hardware);
    void CreateStepMotor(Axis axis, QA_HARDWARE hardware);
    std::shared_ptr<IMotor> GetStepMotor(Axis axis, QA_HARDWARE hardware);
    void CreateActuator(ActuatorType type);
    int RequiredNumberOfMeasurements(double des_std_err_mv, double sd_mv);

    // device containers
    std::shared_ptr<LabJack> labjack_;
    std::shared_ptr<Clock> motor_clock_;
    std::map<Axis, std::shared_ptr<IMotor>> motors_;
    std::map<Axis, std::shared_ptr<ISensor>> encoders_;
    std::map<SensorType, std::shared_ptr<ISensor>> sensors_;
    std::map<ActuatorType, std::shared_ptr<Actuator>> actuators_;

    QMap<Axis, double> max_dev_;
    std::shared_ptr<QARepo> repo_;
    QSettings settings_;
};


#endif
