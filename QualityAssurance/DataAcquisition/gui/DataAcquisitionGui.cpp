#include "DataAcquisitionGui.h"
#include "ui_DataAcquisitionGui.h"

#include "ServoMotor.h"
#include "StepMotor.h"

#include <QMessageBox>
#include <QDebug>
#include <QString>

DataAcquisitionGui::DataAcquisitionGui(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::DataAcquisitionGui),
    settings_(QStandardPaths::locate(QStandardPaths::ConfigLocation, QString("MQA.ini"), QStandardPaths::LocateFile),
                                                          QSettings::IniFormat)
{
    ui_->setupUi(this);
    setWindowTitle("LabJack");
    FillComboBoxes();
    ConnectSignals();
    SetDefaults();
    SetupDb();
    dev_factory_ = std::shared_ptr<DevFactory>(new DevFactory(repo_));
}

DataAcquisitionGui::~DataAcquisitionGui() {}

void DataAcquisitionGui::SetDefaults() {
    // AIN
    ui_->lineEdit_ain_range->setText("10.0");
    ui_->lineEdit_ain_resolution_index->setText("8");
    ui_->lineEdit_ain_single_ended_ch->setText("199");
    ui_->lineEdit_ain_settling_index->setText("0");

    // CLOCK
    ui_->lineEdit_clock_divisor->setText("8");
    ui_->lineEdit_clock_rollvalue->setText("50000");

    // PULSEOUT
    ui_->lineEdit_pulseOut_highToLow_clockCount->setText(QString::number(10000));
    ui_->lineEdit_pulseOut_lowToHigh_clockCount->setText(QString::number(0));
    ui_->lineEdit_pulseOut_pulses->setText(QString::number(2));

    ui_->spinBox_ain_nreads->setValue(1000);

    ui_->lineEdit_ip->setText("172.20.211.128");
}

void DataAcquisitionGui::FillComboBoxes() {
    ui_->comboBox_hardware->addItem(QString("SCANNER2D"), static_cast<int>(QA_HARDWARE::SCANNER2D));
    ui_->comboBox_hardware->addItem(QString("SCANNER3D"), static_cast<int>(QA_HARDWARE::SCANNER3D));
    ui_->comboBox_hardware->addItem(QString("WHEEL"), static_cast<int>(QA_HARDWARE::WHEEL));

    ui_->comboBox_motor_axis->addItem(QString("X"), static_cast<int>(Axis::X));
    ui_->comboBox_motor_axis->addItem(QString("Y"), static_cast<int>(Axis::Y));
    ui_->comboBox_motor_axis->addItem(QString("Z"), static_cast<int>(Axis::Z));

    ui_->comboBox_clocks->addItem(QString("CLOCK0"), static_cast<int>(CLOCK::CLOCK0));
    ui_->comboBox_clocks->addItem(QString("CLOCK1"), static_cast<int>(CLOCK::CLOCK1));
    ui_->comboBox_clocks->addItem(QString("CLOCK2"), static_cast<int>(CLOCK::CLOCK2));
    ui_->comboBox_clocks->setCurrentIndex(static_cast<int>(CLOCK::CLOCK1));

    ui_->comboBox_pulseOut_clock->addItem(QString("CLOCK0"), static_cast<int>(CLOCK::CLOCK0));
    ui_->comboBox_pulseOut_clock->addItem(QString("CLOCK1"), static_cast<int>(CLOCK::CLOCK1));
    ui_->comboBox_pulseOut_clock->addItem(QString("CLOCK2"), static_cast<int>(CLOCK::CLOCK2));
    ui_->comboBox_pulseOut_clock->setCurrentIndex(static_cast<int>(CLOCK::CLOCK1));

    for (int idx = static_cast<int>(AIN::AIN_FIRST); idx <= static_cast<int>(AIN::AIN_LAST); ++idx) {
        ui_->comboBox_ain_channel->addItem(QString("AIN") + QString::number(idx), idx);
    }
    ui_->comboBox_ain_channel->setCurrentIndex(static_cast<int>(AIN::AIN_FIRST));

    ui_->comboBox_aout_channel->addItem(QString("AOUT0"), static_cast<int>(AOUT::AOUT_0));
    ui_->comboBox_aout_channel->addItem(QString("AOUT1"), static_cast<int>(AOUT::AOUT_1));
    ui_->comboBox_aout_channel->setCurrentIndex(static_cast<int>(AOUT::AOUT_0));

    for (int idx = static_cast<int>(DIO::DIO_FIRST); idx <= static_cast<int>(DIO::DIO_LAST); ++idx) {
        ui_->comboBox_dio_channel->addItem(QString("DIO") + QString::number(idx), idx);
    }
    ui_->comboBox_dio_channel->setCurrentIndex(static_cast<int>(DIO::DIO_FIRST));

    ui_->comboBox_pulseOut_channels->addItem("DIO0", static_cast<int>(PULSEOUT::DIO0));
    ui_->comboBox_pulseOut_channels->addItem("DIO2", static_cast<int>(PULSEOUT::DIO2));
    ui_->comboBox_pulseOut_channels->addItem("DIO3", static_cast<int>(PULSEOUT::DIO3));
    ui_->comboBox_pulseOut_channels->addItem("DIO4", static_cast<int>(PULSEOUT::DIO4));
    ui_->comboBox_pulseOut_channels->addItem("DIO5", static_cast<int>(PULSEOUT::DIO5));
    ui_->comboBox_pulseOut_channels->setCurrentIndex(0);

}

void DataAcquisitionGui::SetupHardware() {
    try {
        QA_HARDWARE hardware = static_cast<QA_HARDWARE>(ui_->comboBox_hardware->currentData().toInt());
        qDebug() << "SetupHardware : " << static_cast<int>(hardware);

        dev_factory_->Setup(hardware);
        motors_[Axis::X] = dev_factory_->GetMotor(Axis::X);
        motors_[Axis::Y] = dev_factory_->GetMotor(Axis::Y);
        motors_[Axis::Z] = dev_factory_->GetMotor(Axis::Z);
        QObject::connect(motors_[Axis::X].get(), &IMotor::MovementStarted, this, [=](double act_pos) { (void)act_pos; ui_->lineEdit_state_x->setText("Started");});
        QObject::connect(motors_[Axis::X].get(), &IMotor::MovementAborted, this, [=](QString msg) { (void)msg; ui_->lineEdit_state_x->setText("Aborted");});
        QObject::connect(motors_[Axis::X].get(), &IMotor::MovementFinished, this, [=](double act_pos) { (void)act_pos; ui_->lineEdit_state_x->setText("Finished");});
        QObject::connect(motors_[Axis::X].get(), &IMotor::CurrentPosition,  this, [=](double act_pos) { ui_->lineEdit_pos_x->setText(QString::number(act_pos, 'f', 3)); });

        QObject::connect(motors_[Axis::Y].get(), &IMotor::MovementStarted, this, [=](double act_pos) { (void)act_pos; ui_->lineEdit_state_y->setText("Started");});
        QObject::connect(motors_[Axis::Y].get(), &IMotor::MovementAborted, this, [=](QString msg) { (void)msg; ui_->lineEdit_state_y->setText("Aborted");});
        QObject::connect(motors_[Axis::Y].get(), &IMotor::MovementFinished, this, [=](double act_pos) { (void)act_pos, ui_->lineEdit_state_y->setText("Finished");});
        QObject::connect(motors_[Axis::Y].get(), &IMotor::CurrentPosition,  this, [=](double act_pos) { ui_->lineEdit_pos_y->setText(QString::number(act_pos, 'f', 3)); });

        QObject::connect(motors_[Axis::Z].get(), &IMotor::MovementStarted, this, [=](double act_pos) { (void)act_pos; ui_->lineEdit_state_z->setText("Started");});
        QObject::connect(motors_[Axis::Z].get(), &IMotor::MovementAborted, this, [=](QString msg) { (void)msg; ui_->lineEdit_state_z->setText("Aborted");});
        QObject::connect(motors_[Axis::Z].get(), &IMotor::MovementFinished, this, [=](double act_pos) { (void)act_pos; ui_->lineEdit_state_z->setText("Finished");});
        QObject::connect(motors_[Axis::Z].get(), &IMotor::CurrentPosition,  this, [=](double act_pos) { ui_->lineEdit_pos_z->setText(QString::number(act_pos, 'f', 3)); });
    }
    catch (std::exception& exc) {
        qDebug() << "SetupHardware " << exc.what();
   }
}

void DataAcquisitionGui::DisplayModelessMessageBox(QString msg) {
    qDebug() << "dd";
    QMessageBox box(this);
    box.setText(msg);
    box.setIcon(QMessageBox::Critical);
    box.setModal(false);
    box.setAttribute(Qt::WA_DeleteOnClose);
    box.show();
}

void DataAcquisitionGui::ConnectSignals() {
    // LABJACK
    QObject::connect(ui_->pushButton_connect, &QPushButton::clicked, this, [=]() { labjack_.Connect(ui_->lineEdit_ip->text()); });
    QObject::connect(ui_->pushButton_disconnect, &QPushButton::clicked, &labjack_, &LabJack::Disconnect);

    QObject::connect(ui_->pushButton_load_constants_file, &QPushButton::clicked, &labjack_, &LabJack::LoadConstantsFile);
    QObject::connect(ui_->pushButton_load_configuration_file, &QPushButton::clicked, &labjack_, &LabJack::LoadConfigurationFile);
    QObject::connect(ui_->pushButton_read_configuration, &QPushButton::clicked, &labjack_, &LabJack::ReadConfiguration);

    QObject::connect(ui_->pushButton_load_default_config, &QPushButton::clicked, &labjack_, &LabJack::SetCurrentToDefault);
    QObject::connect(ui_->pushButton_set_as_default_config, &QPushButton::clicked, &labjack_, &LabJack::SetDefaultToCurrent);

    // CLOCK
    QObject::connect(ui_->pushButton_clock_configure, &QPushButton::clicked, [=]() { labjack_.ConfigureClock(
                    CurrentClock(),
                    ui_->lineEdit_clock_divisor->text().toInt(),
                    ui_->lineEdit_clock_rollvalue->text().toInt()); });
    QObject::connect(ui_->pushButton_clock_start, &QPushButton::clicked, this, [=]() { labjack_.StartClock(CurrentClock()); });
    QObject::connect(ui_->pushButton_clock_stop, &QPushButton::clicked, this, [=]() { labjack_.StopClock(CurrentClock()); });
    QObject::connect(ui_->pushButton_clock_read, &QPushButton::clicked, this, [=]() { ui_->lineEdit_clock_value->setText(QString::number(labjack_.ReadClock(CurrentClock()))); });

    // AIN
    QObject::connect(ui_->pushButton_read2AIN, &QPushButton::clicked, this, [=]() {
        double ch1, ch2;
        labjack_.ReadAINS(AIN::AIN0, AIN::AIN1, &ch1, &ch2);
        qDebug() << "AIN0 " << ch1 << " AIN1 " << ch2;
    });
    QObject::connect(ui_->pushButton_ain_read, &QPushButton::clicked, this,[=]() { ui_->lineEdit_ain_value->setText(QString::number(labjack_.ReadAIN(CurrentAIN()), 'f', 3)); });
    QObject::connect(ui_->pushButton_ain_multi_read, &QPushButton::clicked, this, [=]() { labjack_.MultiReadAIN(CurrentAIN(), ui_->spinBox_ain_nreads->value()); });
    QObject::connect(ui_->pushButton_ain_configure, &QPushButton::clicked, this, [=]() { labjack_.ConfigureAIN(
                    CurrentAIN(),
                    ui_->lineEdit_ain_single_ended_ch->text().toInt(),
                    ui_->lineEdit_ain_range->text().toDouble(),
                    ui_->lineEdit_ain_resolution_index->text().toInt(),
                    ui_->lineEdit_ain_settling_index->text().toInt()); });

    // AOUT
//    QObject::connect(ui_->pushButton_read_dac, &QPushButton::clicked, &labjack_, &LabJack::ReadAOUT);
        QObject::connect(ui_->pushButton_aout_write, &QPushButton::clicked, this, [=]() { labjack_.WriteAOUT(CurrentAOUT(), ui_->lineEdit_aout_value->text().toDouble()); });

//    // DIO
    QObject::connect(ui_->pushButton_dio_configure, &QPushButton::clicked, this, [=]() {
       labjack_.ConfigureDIO(static_cast<DIO>(CurrentDIO()), ui_->lineEdit_dio_configure->text().toInt()); });
    QObject::connect(ui_->pushButton_dio_read, &QPushButton::clicked, this, [=]() {
        ui_->lineEdit_dio_read_value->setText(QString::number(labjack_.ReadDIO(CurrentDIO()))); });
    QObject::connect(ui_->pushButton_dio_write, &QPushButton::clicked, this, [=]() {
        labjack_.WriteDIO(CurrentDIO(), ui_->lineEdit_dio_write_value->text().toInt()); });

//    // PULSEOUT
    QObject::connect(ui_->pushButton_pulseOut_configure, &QPushButton::clicked, this, [=]() {
        labjack_.ConfigurePulseOut(CurrentPulseOut(),
                                   CurrentPulseOutClock(),
                                   ui_->lineEdit_pulseOut_highToLow_clockCount->text().toInt(),
                                   ui_->lineEdit_pulseOut_lowToHigh_clockCount->text().toInt(),
                                   ui_->lineEdit_pulseOut_pulses->text().toInt()); });
    QObject::connect(ui_->pushButton_pulseOut_start, &QPushButton::clicked, this, [=](){ labjack_.StartPulseOut(CurrentPulseOut()); });
    QObject::connect(ui_->pushButton_pulseOut_stop, &QPushButton::clicked, this, [=](){ labjack_.StopPulseOut(CurrentPulseOut()); });
    QObject::connect(ui_->pushButton_pulseOut_disable, &QPushButton::clicked, this, [=](){ labjack_.DisablePulseOut(CurrentPulseOut()); });
    QObject::connect(ui_->pushButton_pulseOut_updatePulses, &QPushButton::clicked, this, [=](){ labjack_.UpdatePulseOut(
                    CurrentPulseOut(), ui_->lineEdit_pulseOut_updatePulses->text().toInt()); });
    QObject::connect(ui_->pushButton_pulseOut_readCurrent, &QPushButton::clicked, this, [=](){
        ui_->lineEdit_pulseOut_current->setText(QString::number(labjack_.ReadPulseOutCurrent(CurrentPulseOut()))); });
    QObject::connect(ui_->pushButton_pulseOut_readTarget, &QPushButton::clicked, this, [=](){
        ui_->lineEdit_pulseOut_target->setText(QString::number(labjack_.ReadPulseOutTarget(CurrentPulseOut()))); });

    QObject::connect(ui_->pushButton_pulseOut_start, &QPushButton::clicked, this, [=]() { stopwatch_.start(); });
    QObject::connect(ui_->pushButton_pulseOut_stop, &QPushButton::clicked, this, [=]() { ui_->lineEdit_elapsed_time->setText(QString::number(((double)stopwatch_.elapsed()), 'f', 4)); });


    ///// DEVICE FACTORY
    QObject::connect(ui_->pushButton_setup, &QPushButton::clicked, this, &DataAcquisitionGui::SetupHardware);
    QObject::connect(ui_->pushButton_axis_getposition, &QPushButton::clicked, this, [=]() { try { ui_->lineEdit_axis_position->setText(
                            QString::number(motors_[CurrentAxis()]->ActPos(), 'f', 4)); } catch(std::exception& exc) { qDebug() << QString::fromStdString(exc.what()); } });
    QObject::connect(ui_->pushButton_axis_movetoposition, &QPushButton::clicked, this, [=]() {
        try { motors_[CurrentAxis()]->MoveToPosition(ui_->lineEdit_axis_movetopos->text().toDouble()); } catch (std::exception& exc) { qDebug() << QString::fromStdString(exc.what()); }});
    QObject::connect(ui_->pushButton_axis_movetozero, &QPushButton::clicked, this, [=]() {
        try { motors_[CurrentAxis()]->MoveToZero(); } catch(std::exception& exc) { qDebug() << QString::fromStdString(exc.what()); }});
    QObject::connect(ui_->pushButton_axis_movenegstep, &QPushButton::clicked, this, [=]() {
        try { motors_[CurrentAxis()]->SetNegDir(); motors_[CurrentAxis()]->MoveSingleStep(); } catch (std::exception& exc) { QString::fromStdString(exc.what()); }});
    QObject::connect(ui_->pushButton_axis_preset, &QPushButton::clicked, this, [=]() {
        try { motors_[CurrentAxis()]->MoveToPreset(); } catch (std::exception& exc) { QString::fromStdString(exc.what()); }});

    QObject::connect(ui_->pushButton_axis_moveposstep, &QPushButton::clicked, this, [=]() {
        motors_[CurrentAxis()]->SetPosDir(); motors_[CurrentAxis()]->MoveSingleStep(); });
    QObject::connect(ui_->pushButton_axis_stop, &QPushButton::clicked, this, [=]() {
        try { motors_[CurrentAxis()]->Stop(); } catch (std::exception& exc) { qDebug() << QString::fromStdString(exc.what()); }});
    QObject::connect(ui_->pushButton_axis_settozeropos, &QPushButton::clicked, this, [=]() {
        try { motors_[CurrentAxis()]->SetCurrentToZeroPos(); } catch (std::exception& exc) { qDebug() << QString::fromStdString(exc.what()); }});

    QObject::connect(ui_->pushButton_rapidScanOn, &QPushButton::clicked, this, [=]() { try {
            dev_factory_->GetSensor(SensorType::CHAMBRE)->ConfigureRapidScan(300, 6000);
            dev_factory_->GetSensor(SensorType::DIODE)->ConfigureRapidScan(300, 6000);
            try {
                dev_factory_->GetSensor(SensorType::CHAMBRE)->ReadRapidScan();
            } catch(std::exception& exc) {}
            try {
                dev_factory_->GetSensor(SensorType::DIODE)->ReadRapidScan();
            } catch(std::exception& exc) {}
            dev_factory_->GetSensor(SensorType::CHAMBRE)->ConfigureRapidScan(300, 6000);
            dev_factory_->GetSensor(SensorType::DIODE)->ConfigureRapidScan(300, 6000);
        } catch (std::exception& exc) { qDebug() << QString::fromStdString(exc.what()); } });

    QObject::connect(ui_->pushButton_rapidScanOff, &QPushButton::clicked, this, [=]() { try {
            dev_factory_->GetSensor(SensorType::CHAMBRE)->DisableRapidScan();;
            dev_factory_->GetSensor(SensorType::DIODE)->DisableRapidScan();
        } catch (std::exception& exc) { qDebug() << QString::fromStdString(exc.what()); } });


    QObject::connect(ui_->pushButton_chambre_readRapidScan, &QPushButton::clicked, this, [=]() { try { ui_->lineEdit_chambre_value->setText(QString::number(
           dev_factory_->GetSensor(SensorType::CHAMBRE)->ReadRapidScan(), 'f', 3)); } catch (std::exception& exc) { qDebug() << QString::fromStdString(exc.what()); } });
    QObject::connect(ui_->pushButton_diode_readRapidScan, &QPushButton::clicked, this, [=]() { try { ui_->lineEdit_diode_value->setText(QString::number(
           dev_factory_->GetSensor(SensorType::DIODE)->ReadRapidScan(), 'f', 3)); } catch (std::exception& exc) { qDebug() << QString::fromStdString(exc.what()); } });

    QObject::connect(ui_->pushButton_diode_read, &QPushButton::clicked, this, [=]() { try { ui_->lineEdit_diode_value->setText(QString::number(
           dev_factory_->GetSensor(SensorType::DIODE)->InterpretedValue(), 'f', 3)); } catch (std::exception& exc) { qDebug() << QString::fromStdString(exc.what()); } });
    QObject::connect(ui_->pushButton_chambre_read, &QPushButton::clicked, this, [=]() { try { ui_->lineEdit_chambre_value->setText(QString::number(
            dev_factory_->GetSensor(SensorType::CHAMBRE)->InterpretedValue(), 'f', 3)); } catch(std::exception& exc) { qDebug() << QString::fromStdString(exc.what()); }});
}

void DataAcquisitionGui::MovementStarted(Axis axis, double pos) {
    MovementMessage(axis, pos, "Started");
}

void DataAcquisitionGui::MovementFinished(Axis axis, double pos) {
    MovementMessage(axis, pos, "Finished");
}

void DataAcquisitionGui::MovementAborted(Axis axis, double pos) {
    MovementMessage(axis, pos, "Aborted");
}

void DataAcquisitionGui::CurrentPosition(Axis axis, double pos) {
    MovementMessage(axis, pos, "Moving");
}

void DataAcquisitionGui::MovementMessage(Axis axis, double pos, QString msg) {
    switch (axis) {
    case Axis::X:
        ui_->lineEdit_state_x->setText(msg);
        ui_->lineEdit_pos_x->setText(QString::number(pos, 'f', 1));
        break;
    case Axis::Y:
        ui_->lineEdit_state_y->setText(msg);
        ui_->lineEdit_pos_y->setText(QString::number(pos, 'f', 1));
        break;
    case Axis::Z:
        ui_->lineEdit_state_z->setText(msg);
        ui_->lineEdit_pos_z->setText(QString::number(pos, 'f', 1));
        break;
    default:
        throw std::runtime_error("DataAcquisitionGui::MovementMessage unknown axis");
        break;
    }
}

Axis DataAcquisitionGui::CurrentAxis() {
    return static_cast<Axis>(ui_->comboBox_motor_axis->currentData().toInt());
}

CLOCK DataAcquisitionGui::CurrentClock() {
    return static_cast<CLOCK>(ui_->comboBox_clocks->currentData().toInt());
}

AIN DataAcquisitionGui::CurrentAIN() {
    return static_cast<AIN>(ui_->comboBox_ain_channel->currentData().toInt());
}

AOUT DataAcquisitionGui::CurrentAOUT() {
    return static_cast<AOUT>(ui_->comboBox_aout_channel->currentData().toInt());
}

DIO DataAcquisitionGui::CurrentDIO() {
    return static_cast<DIO>(ui_->comboBox_dio_channel->currentData().toInt());
}

PULSEOUT DataAcquisitionGui::CurrentPulseOut() {
    return static_cast<PULSEOUT>(ui_->comboBox_pulseOut_channels->currentData().toInt());
}

CLOCK DataAcquisitionGui::CurrentPulseOutClock() {
    return static_cast<CLOCK>(ui_->comboBox_pulseOut_clock->currentData().toInt());
}

void DataAcquisitionGui::SetupDb() {
    QString dbHostName = settings_.value("qadatabase/host", "unknown").toString();
    QString dbDatabaseName = settings_.value("qadatabase/name", "unknown").toString();
    int dbPort = settings_.value("qadatabase/port", "unknown").toInt();
    QString dbUserName = settings_.value("qadatabase/user", "unknown").toString();
    QString dbPassword = settings_.value("qadatabase/password", "unknown").toString();
    QString dbConnName = "ptdb";

    if (dbHostName == "unknown") qCritical() << "MQA::SetupDb Unknown db hostname";
    if (dbDatabaseName == "unknown") qCritical() << "MQA::SetupDb Unknown db name";
    if (dbUserName == "unknown") qCritical() << "MQA::SetupDb Unknown db username";
    if (dbPassword == "unknown") qCritical() << "MQA::SetupDb Unknown db password";

    if (!repo_) {
        repo_ = std::shared_ptr<QARepo>(new QARepo(dbConnName, dbHostName, dbPort, dbDatabaseName, dbUserName, dbPassword));
    }

    repo_->Connect();
}




