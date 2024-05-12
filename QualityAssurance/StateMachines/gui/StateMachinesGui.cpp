#include "StateMachinesGui.h"
#include "ui_StateMachinesGui.h"

#include <QMessageBox>
#include <QDebug>

StateMachinesGui::StateMachinesGui(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::StateMachinesGui),
    settings_(QStandardPaths::locate(QStandardPaths::ConfigLocation, QString("MQA.ini"), QStandardPaths::LocateFile),
                                                          QSettings::IniFormat)
{
    ui_->setupUi(this);
    setWindowTitle("Axis Scanner");
    position_accuracy_ = std::shared_ptr<Histogram>(new Histogram(ui_->customPlot_pos_accuracy, "", "", -0.5, 0.5, 100));
    SetDefaults();
    FillComboBoxes();
    SetupDb();
    ConnectSignals();
}

StateMachinesGui::~StateMachinesGui() {
    file->close();
}

void StateMachinesGui::SetDefaults() {
    ui_->lineEdit_axisScan_start->setText("-3.0");
    ui_->lineEdit_axisScan_end->setText("3.0");
    ui_->lineEdit_axisScan_step->setText("0.1");
    ui_->lineEdit_axisScan_measurements->setText("1");

    ui_->lineEdit_axisStep_start->setText("-3.0");
    ui_->lineEdit_axisStep_end->setText("3.0");
    ui_->lineEdit_axisStep_step->setText("0.1");
}

void StateMachinesGui::FillComboBoxes() {
    ui_->comboBox_axisScan_motor_axis->addItem(QString("X"), static_cast<int>(Axis::X));
    ui_->comboBox_axisScan_motor_axis->addItem(QString("Y"), static_cast<int>(Axis::Y));
    ui_->comboBox_axisScan_motor_axis->addItem(QString("Z"), static_cast<int>(Axis::Z));
    ui_->comboBox_axisStep_motor_axis->addItem(QString("X"), static_cast<int>(Axis::X));
    ui_->comboBox_axisStep_motor_axis->addItem(QString("Y"), static_cast<int>(Axis::Y));
    ui_->comboBox_axisStep_motor_axis->addItem(QString("Z"), static_cast<int>(Axis::Z));

    ui_->comboBox_hardware->addItem(QString("SCANNER3D"), static_cast<int>(QA_HARDWARE::SCANNER3D));
    ui_->comboBox_hardware->addItem(QString("SCANNER2D"), static_cast<int>(QA_HARDWARE::SCANNER2D));
    ui_->comboBox_hardware->addItem(QString("WHEEL"), static_cast<int>(QA_HARDWARE::WHEEL));
}

QString StateMachinesGui::VectorToString(const std::vector<double> data) {
    QString str;
    for (auto d: data) {
        str.append(QString::number(d, 'f', 3) + ", ");
    }
    str.append("\n");
    return str;
}

void StateMachinesGui::ConfigureHardware() {
    try {
        file = new QFile("/home/cyclo/Measurement.csv");
        file->open(QFile::WriteOnly);
        stream = new QTextStream(file);
    dev_factory_ = std::shared_ptr<DevFactory>(new DevFactory(repo_));
    QA_HARDWARE hardware = static_cast<QA_HARDWARE>(ui_->comboBox_hardware->currentData().toInt());
    qDebug() << "SetupHardware : " << static_cast<int>(hardware);
    dev_factory_->Setup(hardware);
    axis_scanners_[Axis::X] = std::shared_ptr<AxisScan>(new AxisScan(Axis::X, dev_factory_, &beacon_));
    axis_scanners_[Axis::Y] = std::shared_ptr<AxisScan>(new AxisScan(Axis::Y, dev_factory_, &beacon_));
    axis_scanners_[Axis::Z] = std::shared_ptr<AxisScan>(new AxisScan(Axis::Z, dev_factory_, &beacon_));
    axis_steppers_[Axis::X] = std::shared_ptr<AxisStepper>(new AxisStepper(Axis::X, dev_factory_, &beacon_));
    axis_steppers_[Axis::Y] = std::shared_ptr<AxisStepper>(new AxisStepper(Axis::Y, dev_factory_, &beacon_));
    axis_steppers_[Axis::Z] = std::shared_ptr<AxisStepper>(new AxisStepper(Axis::Z, dev_factory_, &beacon_));
    motors_[Axis::X] = dev_factory_->GetMotor(Axis::X);
    motors_[Axis::Y] = dev_factory_->GetMotor(Axis::Y);
    motors_[Axis::Z] = dev_factory_->GetMotor(Axis::Z);
    QObject::connect(motors_[Axis::X].get(), &IMotor::MovementStarted, this, [=](double act_pos) { (void)act_pos; ui_->lineEdit_state_x->setText("Started");});
    QObject::connect(motors_[Axis::X].get(), &IMotor::MovementAborted, this, [=](QString msg) { (void)msg; ui_->lineEdit_state_x->setText("Aborted");});
    QObject::connect(motors_[Axis::X].get(), &IMotor::MovementFinished, this, [=](double act_pos) { (void)act_pos;ui_->lineEdit_state_x->setText("Finished");});
    QObject::connect(motors_[Axis::X].get(), &IMotor::CurrentPosition,  this, [=](double act_pos) { ui_->lineEdit_pos_x->setText(QString::number(act_pos, 'f', 3)); });

    QObject::connect(motors_[Axis::Y].get(), &IMotor::MovementStarted, this, [=](double act_pos) { (void)act_pos; ui_->lineEdit_state_y->setText("Started");});
    QObject::connect(motors_[Axis::Y].get(), &IMotor::MovementAborted, this, [=](QString msg) { (void)msg; ui_->lineEdit_state_y->setText("Aborted");});
    QObject::connect(motors_[Axis::Y].get(), &IMotor::MovementFinished, this, [=](double act_pos) { (void)act_pos; ui_->lineEdit_state_y->setText("Finished");});
    QObject::connect(motors_[Axis::Y].get(), &IMotor::CurrentPosition,  this, [=](double act_pos) { ui_->lineEdit_pos_y->setText(QString::number(act_pos, 'f', 3)); });

    QObject::connect(motors_[Axis::Z].get(), &IMotor::MovementStarted, this, [=](double act_pos) { (void)act_pos; ui_->lineEdit_state_z->setText("Started");});
    QObject::connect(motors_[Axis::Z].get(), &IMotor::MovementAborted, this, [=](QString msg) { (void)msg; ui_->lineEdit_state_z->setText("Aborted");});
    QObject::connect(motors_[Axis::Z].get(), &IMotor::MovementFinished, this, [=](double act_pos) { (void)act_pos; ui_->lineEdit_state_z->setText("Finished");});
    QObject::connect(motors_[Axis::Z].get(), &IMotor::CurrentPosition,  this, [=](double act_pos) { (void)act_pos; ui_->lineEdit_pos_z->setText(QString::number(act_pos, 'f', 3)); });
    QObject::connect(axis_scanners_[Axis::X].get(), &AxisScan::Ready, this, [=](bool yes) { ui_->plainTextEdit_scanning_progress->appendPlainText(QString("X READY ") + QString::number(yes)); });
    QObject::connect(axis_scanners_[Axis::X].get(), &AxisScan::Finished, this, [=]() { ui_->plainTextEdit_scanning_progress->appendPlainText(QString("X FINISHED")); });
    QObject::connect(axis_scanners_[Axis::X].get(), &AxisScan::MeasurementAll, this, [=](double pos, std::vector<double> value) { *stream << QString::number(pos, 'f', 3) << QString(", ") << VectorToString(value); } );
    QObject::connect(axis_scanners_[Axis::Y].get(), &AxisScan::MeasurementAll, this, [=](double pos, std::vector<double> value) { *stream << QString::number(pos, 'f', 3) << ", " << VectorToString(value); } );
    QObject::connect(axis_scanners_[Axis::Z].get(), &AxisScan::MeasurementAll, this, [=](double pos, std::vector<double> value) { *stream << QString::number(pos, 'f', 3) << ", " << VectorToString(value); } );


//    QObject::connect(axis_scanners_[Axis::X].get(), &AxisScan::Measurement, this, [=](double pos, Signal signal) { ui_->plainTextEdit_scanning_progress->appendPlainText(
//                    QString("X POS ") + QString::number(pos, 'f', 2) + QString(" Diode ") + QString::number(signal.raw(), 'f', 2) + QString(" Ref ") + QString::number(signal.reference(), 'f', 2)); });
    QObject::connect(axis_scanners_[Axis::X].get(), &AxisScan::MovementFinished, this, [=](double act_pos, double des_pos) { ui_->plainTextEdit_scanning_progress->appendPlainText(
                    QString("X ACT ") + QString::number(act_pos, 'f', 2) + QString(" DES ") + QString::number(des_pos, 'f', 2) + QString(" DIFF ") + QString::number(act_pos - des_pos, 'f', 3));
                    position_accuracy_->SetSingleElement(act_pos - des_pos); });

    QObject::connect(axis_scanners_[Axis::Y].get(), &AxisScan::Ready, this, [=](bool yes) { ui_->plainTextEdit_scanning_progress->appendPlainText(QString("Y READY ") + QString::number(yes)); });
    QObject::connect(axis_scanners_[Axis::Y].get(), &AxisScan::Finished, this, [=]() { ui_->plainTextEdit_scanning_progress->appendPlainText(QString("Y FINISHED")); });
//    QObject::connect(axis_scanners_[Axis::Y].get(), &AxisScan::Measurement, this, [=](double pos, Signal signal) { ui_->plainTextEdit_scanning_progress->appendPlainText(
//                    QString("Y POS ") + QString::number(pos, 'f', 2) + QString(" Diode ") + QString::number(signal.raw(), 'f', 2) + QString(" Ref ") + QString::number(signal.reference(), 'f', 2)); });
    QObject::connect(axis_scanners_[Axis::Y].get(), &AxisScan::MovementFinished, this, [=](double act_pos, double des_pos) { ui_->plainTextEdit_scanning_progress->appendPlainText(
                    QString("Y ACT ") + QString::number(act_pos, 'f', 2) + QString(" DES ") + QString::number(des_pos, 'f', 2) + QString(" DIFF ") + QString::number(act_pos - des_pos, 'f', 3));
                    position_accuracy_->SetSingleElement(act_pos - des_pos); });

    QObject::connect(axis_scanners_[Axis::Z].get(), &AxisScan::Ready, this, [=](bool yes) { ui_->plainTextEdit_scanning_progress->appendPlainText(QString("Z READY ") + QString::number(yes)); });
    QObject::connect(axis_scanners_[Axis::Z].get(), &AxisScan::Finished, this, [=]() { ui_->plainTextEdit_scanning_progress->appendPlainText(QString("Z FINISHED")); });
//    QObject::connect(axis_scanners_[Axis::Z].get(), &AxisScan::Measurement, this, [=](double pos, Signal signal) { ui_->plainTextEdit_scanning_progress->appendPlainText(
//                    QString("Z POS ") + QString::number(pos, 'f', 2) + QString(" Diode ") + QString::number(signal.raw(), 'f', 2) + QString(" Ref ") + QString::number(signal.reference(), 'f', 2)); });
    QObject::connect(axis_scanners_[Axis::Z].get(), &AxisScan::MovementFinished, this, [=](double act_pos, double des_pos) { ui_->plainTextEdit_scanning_progress->appendPlainText(
                    QString("Z ACT ") + QString::number(act_pos, 'f', 2) + QString(" DES ") + QString::number(des_pos, 'f', 2) + QString(" DIFF ") + QString::number(act_pos - des_pos, 'f', 3));
                    position_accuracy_->SetSingleElement(act_pos - des_pos); });

    QObject::connect(axis_steppers_[Axis::X].get(), &AxisStepper::Ready, this, [=](bool yes) { ui_->plainTextEdit_scanning_progress->appendPlainText(QString("X READY ") + QString::number(yes)); });
    QObject::connect(axis_steppers_[Axis::X].get(), &AxisStepper::Finished, this, [=]() { ui_->plainTextEdit_scanning_progress->appendPlainText(QString("X FINISHED")); });
    QObject::connect(axis_steppers_[Axis::X].get(), &AxisStepper::MovementFinished, this, [=](double act_pos, double des_pos) { ui_->plainTextEdit_scanning_progress->appendPlainText(
                    QString("X ACT ") + QString::number(act_pos, 'f', 2) + QString(" DES ") + QString::number(des_pos, 'f', 2) + QString(" DIFF ") + QString::number(act_pos - des_pos, 'f', 3));
                    position_accuracy_->SetSingleElement(act_pos - des_pos); });

    QObject::connect(axis_steppers_[Axis::Y].get(), &AxisStepper::Ready, this, [=](bool yes) { ui_->plainTextEdit_scanning_progress->appendPlainText(QString("Y READY ") + QString::number(yes)); });
    QObject::connect(axis_steppers_[Axis::Y].get(), &AxisStepper::Finished, this, [=]() { ui_->plainTextEdit_scanning_progress->appendPlainText(QString("Y FINISHED")); });
    QObject::connect(axis_steppers_[Axis::Y].get(), &AxisStepper::MovementFinished, this, [=](double act_pos, double des_pos) { ui_->plainTextEdit_scanning_progress->appendPlainText(
                    QString("Y ACT ") + QString::number(act_pos, 'f', 2) + QString(" DES ") + QString::number(des_pos, 'f', 2) + QString(" DIFF ") + QString::number(act_pos - des_pos, 'f', 3));
                    position_accuracy_->SetSingleElement(act_pos - des_pos); });

    QObject::connect(axis_steppers_[Axis::Z].get(), &AxisStepper::Ready, this, [=](bool yes) { ui_->plainTextEdit_scanning_progress->appendPlainText(QString("Z READY ") + QString::number(yes)); });
    QObject::connect(axis_steppers_[Axis::Z].get(), &AxisStepper::Finished, this, [=]() { ui_->plainTextEdit_scanning_progress->appendPlainText(QString("Z FINISHED")); });
    QObject::connect(axis_steppers_[Axis::Z].get(), &AxisStepper::MovementFinished, this, [=](double act_pos, double des_pos) { ui_->plainTextEdit_scanning_progress->appendPlainText(
                    QString("Z ACT ") + QString::number(act_pos, 'f', 2) + QString(" DES ") + QString::number(des_pos, 'f', 2) + QString(" DIFF ") + QString::number(act_pos - des_pos, 'f', 3));
                    position_accuracy_->SetSingleElement(act_pos - des_pos); });

    }
    catch (std::exception& exc) {
        qDebug() << "ConfigureHardware Exception: " << QString::fromStdString(exc.what());
    }
}

void StateMachinesGui::ConnectSignals() {
    QObject::connect(ui_->pushButton_configure, &QPushButton::clicked, this, &StateMachinesGui::ConfigureHardware);
    QObject::connect(&beacon_, &Beacon::BroadcastedMessage, ui_->plainTextEdit_beacon_messages, &QPlainTextEdit::appendPlainText);


    QObject::connect(ui_->pushButton_axisScan_abort, &QPushButton::clicked, &beacon_, &Beacon::Activate);
    QObject::connect(ui_->pushButton_axisScan_init, &QPushButton::clicked, this, [=]() { axis_scanners_[CurrentScanAxis()]->Init(ui_->lineEdit_axisScan_start->text().toDouble(),
                                                                                                      ui_->lineEdit_axisScan_end->text().toDouble(),
                                                                                                      ui_->lineEdit_axisScan_step->text().toDouble(),
                                                                                                      ui_->lineEdit_axisScan_measurements->text().toInt(),
                                                                                                      ui_->lineEdit_axisScan_max_se->text().toDouble(),
                                                                                                      ui_->radioButton_axisScan_fixed_n->isChecked()); });

    QObject::connect(ui_->pushButton_axisScan_start, &QPushButton::clicked, this, [=]() { axis_scanners_[CurrentScanAxis()]->Start(); });
    QObject::connect(ui_->pushButton_axisScan_invert, &QPushButton::clicked, this, [=]() { axis_scanners_[CurrentScanAxis()]->InvertDirection(); });

    QObject::connect(ui_->pushButton_axisStep_abort, &QPushButton::clicked, &beacon_, &Beacon::Activate);
    QObject::connect(ui_->pushButton_axisStep_init, &QPushButton::clicked, this, [=]() { axis_steppers_[CurrentStepAxis()]->Init(ui_->lineEdit_axisStep_start->text().toDouble(),
                                                                                                      ui_->lineEdit_axisStep_end->text().toDouble(),
                                                                                                      ui_->lineEdit_axisStep_step->text().toDouble()); });
    QObject::connect(ui_->pushButton_axisStep_step, &QPushButton::clicked, this, [=]() { axis_steppers_[CurrentStepAxis()]->MoveStep(); });
    QObject::connect(ui_->pushButton_axisStep_invert, &QPushButton::clicked, this, [=]() { axis_steppers_[CurrentStepAxis()]->InvertDirection(); });

    QObject::connect(ui_->pushButton_clear_histogram, &QPushButton::clicked, [=]() { position_accuracy_->Clear(); });


}

Axis StateMachinesGui::CurrentScanAxis() {
    return static_cast<Axis>(ui_->comboBox_axisScan_motor_axis->currentData().toInt());
}

Axis StateMachinesGui::CurrentStepAxis() {
    return static_cast<Axis>(ui_->comboBox_axisStep_motor_axis->currentData().toInt());
}

void StateMachinesGui::SetupDb() {
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

