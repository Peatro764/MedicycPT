#include "DevFactory.h"

#include <QDebug>
#include <QStandardPaths>

#include "StepMotor.h"
#include "ServoMotor.h"
#include "Sensor.h"
#include "PositionSensor.h"
#include "Axis.h"
#include "Hardware.h"
#include "Calc.h"

DevFactory::DevFactory(std::shared_ptr<QARepo> repo)
   : repo_(repo),
     settings_(QStandardPaths::locate(QStandardPaths::ConfigLocation, QString("MQA.ini"), QStandardPaths::LocateFile),
              QSettings::IniFormat)
{
    qDebug() << "DevFactory";
    max_dev_[Axis::X] = settings_.value("scanner3D/max_x_pos_deviation", 0.2).toDouble();
    max_dev_[Axis::Y] = settings_.value("scanner3D/max_y_pos_deviation", 0.2).toDouble();
    max_dev_[Axis::Z] = settings_.value("scanner3D/max_z_pos_deviation", 0.1).toDouble();
}

DevFactory::~DevFactory() {
    qDebug() << "~DevFactory";
    if (labjack_) labjack_->Disconnect();
}

bool DevFactory::IsConnected() const {
    if (labjack_) {
        return labjack_->IsConnected();
    } else {
        return false;
    }
}

void DevFactory::Setup(QA_HARDWARE hardware) {
    CreateLabJack();
    ConnectLabJack();
    CreateDevices(hardware);
    ConfigureDevices();
}

void DevFactory::CleanUp() {
    DisconnectLabJack();
}

std::shared_ptr<IMotor> DevFactory::GetMotor(Axis axis) {
    if (motors_.find(axis) == motors_.end()) {
        throw std::runtime_error(std::string("No motor configured for axis: ") + std::to_string(static_cast<int>(axis)));
    }
    return motors_.at(axis);
}

std::shared_ptr<ISensor> DevFactory::GetEncoder(Axis axis) {
    if (encoders_.find(axis) == encoders_.end()) {
        throw std::runtime_error(std::string("No encoder configured for axis: ") + std::to_string(static_cast<int>(axis)));
    }
    return encoders_.at(axis);
}

std::shared_ptr<ISensor> DevFactory::GetSensor(SensorType type) {
    if (sensors_.find(type) == sensors_.end()) {
        throw std::runtime_error(std::string("No sensor configured for type: ") + std::to_string(static_cast<int>(type)));
    }
    return sensors_.at(type);
}

std::shared_ptr<Actuator> DevFactory::GetActuator(ActuatorType type) {
    if (actuators_.find(type) == actuators_.end()) {
        throw std::runtime_error(std::string("No actuator configured for type: ") + std::to_string(static_cast<int>(type)));
    }
    return actuators_.at(type);
}

void DevFactory::CreateLabJack() {
    qDebug() << "DevFactory::CreateLabJack";
    if (!labjack_) {
        qDebug() << "DevFactory::CreateLabJack Creating new";
        labjack_ = std::shared_ptr<LabJack>(new LabJack());
    }
}

void DevFactory::ConnectLabJack() {
    qDebug() << "DevFactory::ConnectLabJack";
    QString ip = settings_.value("labjack/ip", "LJM_idANY").toString();
    qDebug() << "DevFactory::ConnectLabJack IP " << ip;
    bool usb = settings_.value("labjack/usb", "False").toBool();
    qDebug() << "DevFactory::ConnectLabJack Use USB: " << usb;
    if (usb) {
        labjack_->ConnectUSB();
    } else {
        labjack_->Connect(ip); // throws if it fails to connect
    }
    qDebug() << "DevFactory::Connected";
}

void DevFactory::DisconnectLabJack() {
    if (labjack_) {
        labjack_->Disconnect();
    }
}

//std::vector<double> DevFactory::ReadMultipleSensors(SensorType type1, SensorType type2, int n_measurements) {
//    return labjack_->ReadAINS(GetSensor(type1)->ch(), GetSensor(type2)->ch(), n_measurements);
//}

int DevFactory::RequiredNumberOfMeasurements(double des_std_err_mv, double sd_mv) {
    return static_cast<int>(std::round(std::pow(sd_mv / des_std_err_mv, 2)));
}

std::vector<double> DevFactory::ReadMultipleSensors(SensorType type1, SensorType type2,
                                                    int n_measurements, double max_std_err, bool fixed_nmb_measurements) {
    std::vector<double> ch1_vector;
    std::vector<double> ch2_vector;
    double ch1_value = 0;
    double ch2_value = 0;
    AIN ch1_name = GetSensor(type1)->ch();
    AIN ch2_name = GetSensor(type2)->ch();
    for (int idx = 0; idx < n_measurements; ++idx) {
        labjack_->ReadAINS(ch1_name, ch2_name, &ch1_value, &ch2_value);
        ch1_vector.push_back(ch1_value);
        ch2_vector.push_back(ch2_value);
    }

    if (fixed_nmb_measurements) {
        return std::vector<double> { calc::Mean(ch1_vector), calc::Mean(ch2_vector) };
    }

    const double sd_max_mv = 1000.0 * std::max(calc::StdDev(ch1_vector), calc::StdDev(ch2_vector));
    const int n_additional_measurements = std::min(100, RequiredNumberOfMeasurements(max_std_err, sd_max_mv) - n_measurements);
//    qDebug() << "DevFactory::ReadMultipleSensors Max SD " << sd_max_mv << " DES SE " << max_std_err << " N MEAS " << n_additional_measurements;
    if (n_additional_measurements <= 0) {
        return std::vector<double> { calc::Mean(ch1_vector), calc::Mean(ch2_vector) };
    }

    for (int idx = 0; idx < n_additional_measurements; ++idx) {
        labjack_->ReadAINS(ch1_name, ch2_name, &ch1_value, &ch2_value);
        ch1_vector.push_back(ch1_value);
        ch2_vector.push_back(ch2_value);
    }
    return std::vector<double> { calc::Mean(ch1_vector), calc::Mean(ch2_vector) };
}

void DevFactory::CreateDevices(QA_HARDWARE hardware) {
    qDebug() << "DevFactory::CreateDevices";
    CreateClock(hardware);
    CreateSensor(SensorType::CHAMBRE);
    CreateSensor(SensorType::DIODE);
    CreateActuator(ActuatorType::AOUT0); // TODO: Remove when simulation not needed

    switch(hardware) {
    case QA_HARDWARE::SCANNER3D:
//        CreateServoMotor(Axis::X, QA_HARDWARE::SCANNER3D);
//        CreateServoMotor(Axis::Y, QA_HARDWARE::SCANNER3D);
//        CreateServoMotor(Axis::Z, QA_HARDWARE::SCANNER3D);
        CreateStepMotor(Axis::X, QA_HARDWARE::SCANNER3D);
        CreateStepMotor(Axis::Y, QA_HARDWARE::SCANNER3D);
        CreateStepMotor(Axis::Z, QA_HARDWARE::SCANNER3D);
        CreateEncoder(Axis::X);
        CreateEncoder(Axis::Y);
        CreateEncoder(Axis::Z);
        break;
    case QA_HARDWARE::WHEEL:
        CreateStepMotor(Axis::X, QA_HARDWARE::SCANNER2D); // NOT USED
        CreateStepMotor(Axis::Y, QA_HARDWARE::SCANNER2D); // NOT USED
        CreateStepMotor(Axis::Z, QA_HARDWARE::WHEEL);
        break;
    case QA_HARDWARE::SCANNER2D:
        CreateStepMotor(Axis::X, QA_HARDWARE::SCANNER2D);
        CreateStepMotor(Axis::Y, QA_HARDWARE::SCANNER2D);
        CreateStepMotor(Axis::Z, QA_HARDWARE::SCANNER2D);
        break;
    default:
        throw std::runtime_error("Unknown hardware option");
        break;
    }
    qDebug() << "DevFactory::CreateDevices Done";
}

void DevFactory::ConfigureDevices() {
//    if (!labjack_) { throw std::runtime_error("LabJack object not created"); }
    labjack_->LoadConfigurationFile();
    labjack_->LoadConstantsFile();

    if (!motor_clock_) { throw std::runtime_error("Clock object not created"); }
    motor_clock_->Configure();
    motor_clock_->Start();

    for (auto s: sensors_) {
        qDebug() << "DevFactory::ConfigureDevices Sensor " << static_cast<int>(s.first);
        s.second->Configure();
    }
    for (auto m: motors_) {
        qDebug() << "DevFactory::ConfigureDevices Motor " << static_cast<int>(m.first);
        m.second->Configure();
    }
    for (auto e: encoders_) {
        qDebug() << "DevFactory::ConfigureEncoder " << static_cast<int>(e.first);
        e.second->Configure();
    }

    qDebug() << "DevFactory::ConfigureDevices Done";
}

void DevFactory::CreateClock(QA_HARDWARE hardware) {
    if (!motor_clock_) {
        qDebug() << "DevFactory::CreateClock Creating new";
        motor_clock_ = std::shared_ptr<Clock>(new Clock(labjack_, repo_->GetClockConfig(hardware)));
    } else {
        qDebug() << "DevFactory::CreateClock Already created, skipping";
    }
}

void DevFactory::CreateActuator(ActuatorType type) {
    if (actuators_.find(type) == actuators_.end()) {
        qDebug() << "DeviceFactor::CreateActuator Creating new";
        actuators_[ActuatorType::AOUT0] = std::shared_ptr<Actuator>(new Actuator(labjack_, AOUT::AOUT_0)); // TODO remove when not needed in simulations
    } else {
        qDebug() << "DeviceFactor::CreateActuator Already created, skipping";
    }
}

void DevFactory::CreateSensor(SensorType type) {
    if (sensors_.find(type) == sensors_.end()) {
        qDebug() << "DeviceFactor::CreateSensor Creating new";
        sensors_[type] = std::shared_ptr<ISensor>(new Sensor(labjack_, repo_->GetSensorConfig(type)));
    } else {
        qDebug() << "DeviceFactor::CreateSensor Already created, skipping";
    }
}

void DevFactory::CreateEncoder(Axis axis) {
    if (encoders_.find(axis) == encoders_.end()) {
        qDebug() << "DevFactory::CreateEncoder Creating new";
        encoders_[axis] = std::shared_ptr<ISensor>(new PositionSensor(labjack_, repo_->GetSensorConfig(axis)));
    } else {
        qDebug() << "DevFactory::CreateEncoder Already created, skipping";
    }
}

void DevFactory::CreateServoMotor(Axis axis, QA_HARDWARE hardware) {
    if (motors_.find(axis) == motors_.end()) {
        qDebug() << "DevFactory::CreateServoMotor Creating new servomotor Axis " << AxisToString(axis) << " Type " << HardwareToString(hardware);
        CreateEncoder(axis);
        motors_[axis] = std::shared_ptr<IMotor> (new ServoMotor(GetStepMotor(axis, hardware),
                                                                encoders_.at(axis),
                                                                GetAllowedRange(axis)));
    } else {
        qDebug() << "DevFactory::CreateMotor Already created, skipping";
    }
}

void DevFactory::CreateStepMotor(Axis axis, QA_HARDWARE hardware) {
    if (motors_.find(axis) == motors_.end()) {
        qDebug() << "DevFactory::CreateMotor Creating new stepmotor Axis " << AxisToString(axis) << " Type " << HardwareToString(hardware);
        motors_[axis] = std::shared_ptr<IMotor>(new StepMotor(labjack_, motor_clock_,
                                                              repo_->GetMotorConfig(axis, hardware), GetAllowedRange(axis)));
    } else {
        qDebug() << "DevFactory::CreateMotor Already created, skipping";
    }
}

std::shared_ptr<IMotor> DevFactory::GetStepMotor(Axis axis, QA_HARDWARE hardware) {
    return std::shared_ptr<IMotor>(new StepMotor(labjack_, motor_clock_, repo_->GetMotorConfig(axis, hardware), GetAllowedRange(axis)));
}

Range DevFactory::GetAllowedRange(Axis axis) const {
    QString min_param = "allowed_range/" + AxisToString(axis).toLower() + "_min";
    QString max_param = "allowed_range/" + AxisToString(axis).toLower() + "_max";
    double min = settings_.value(min_param, "0.0").toDouble();
    double max = settings_.value(max_param, "0.0").toDouble();
    return Range(min, max);
}

void DevFactory::CalibrateAxisPosition(Axis axis) {
    double pos = this->GetEncoder(axis)->InterpretedValue(30, 2);
    qDebug() << "DevFactory::CalibrateAxisPosition Axis " << AxisToString(axis) << " Encoder " << pos << " Motor " << this->GetMotor(axis)->ActPos();
    Range range = GetAllowedRange(axis);
    if (!range.IsInside(pos)) {
        throw std::runtime_error("Out-of-range position returned from position sensor");
    }
    this->GetMotor(axis)->SetCurrentPos(pos);
}

bool DevFactory::VerifyAxisPositionCalibration(Axis axis, double& diff) {
    double sensor_pos = this->GetEncoder(axis)->InterpretedValue(30, 2);
    double motor_pos = this->GetMotor(axis)->ActPos();
    diff = (sensor_pos - motor_pos);
    if (!max_dev_.contains(axis)) {
        qWarning() << "DevFactory::VerifyAxisPositionCalibration Axis does not exist in map " << AxisToString(axis);
        return true;
    } else {
        return (fabs(diff) < max_dev_[axis]);
    }
}


