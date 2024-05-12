#include "PatientDebitGui.h"

#include <QMessageBox>
#include <cstdlib>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <set>
#include <QStandardPaths>
#include "unistd.h"

#include "ui_MQA.h"
#include "MQA.h"
#include "ManualMotorControlDialog.h"
#include "Chambre.h"
#include "DoseRateMeasurementDialog.h"
#include "DoseRateSettingsDialog.h"
#include "Modulateur.h"
#include "Degradeur.h"
#include "Util.h"
#include "Calc.h"
#include "SOBPMaker.h"
#include "SeanceParameterSelectorDialog.h"
#include "DossierSelectionDialog.h"
#include "TimedState.h"

PatientDebitGui::PatientDebitGui(MQA *parent, std::shared_ptr<PTRepo> pt_repo, std::shared_ptr<QARepo> qa_repo) :
    GuiBase(parent),
    settings_(std::unique_ptr<QSettings>(new QSettings(QStandardPaths::locate(QStandardPaths::ConfigLocation, QString("MQA.ini"), QStandardPaths::LocateFile),
                                                          QSettings::IniFormat))),
    pt_repo_(pt_repo),
    qa_repo_(qa_repo),
    nj1_("MQA", false)
{
    dev_factory_ = std::shared_ptr<DevFactory>(new DevFactory(qa_repo_));
    InitTables();
    InitProgressBar();
    InitChambres();
    SetupDBus();
    ConnectSignals();
    SetupLineEditValidators();
    SetupStateMachine();
}

PatientDebitGui::~PatientDebitGui() {
    if (radiationmonitor_interface_) {
        delete radiationmonitor_interface_;
    }
    TurnOffConnectedButton();
}

void PatientDebitGui::TurnOffConnectedButton() {
    parent_->ui()->debit_pushButton_connected->setStyleSheet(ButtonStyleSheet("connected_red_button.png"));
}

void PatientDebitGui::TurnOnConnectedButton() {
    parent_->ui()->debit_pushButton_connected->setStyleSheet(ButtonStyleSheet("connected_green_button.png"));
}

void PatientDebitGui::SetupDBus() {
    QString dbus_host = settings_->value("dbus/host", "").toString();
    QString dbus_port = settings_->value("dbus/port", "").toString();
    QString dbus_name = settings_->value("dbus/name", "").toString();
    QString dbus_conn = QString("tcp:host=%1,port=%2").arg(dbus_host).arg(dbus_port);
    qDebug() << "PatientDebitGui::SetupDBus "
             << "Host " << dbus_host
             << "Port " << dbus_port
             << "Name " << dbus_name
             << "Full connection name " << dbus_conn;
    QDBusConnection dbus_connection = QDBusConnection::connectToBus(dbus_conn, dbus_name);
    if (dbus_connection.isConnected()) {
        qDebug() << "PatientDebitGui::SetupDBus DBus connection established";
    } else {
        qWarning() << "PatientDebitGui::SetupDBus DBus connection failed";
        DisplayModelessMessageBox("La connexion au bus dbus a échoué", true, 5000, QMessageBox::Critical);
    }
    radiationmonitor_interface_ = new medicyc::cyclotron::RadiationMonitorInterface("medicyc.cyclotron.hardware.radiation_monitor.RadiationMonitor_Salle_Clinic",
                                                                                    "/RadiationMonitor_Salle_Clinic", dbus_connection, this);
    if (radiationmonitor_interface_->isValid()) {
        qDebug() << "PatientDebitGui::SetupDBus Interface connection valid";
    } else {
        qWarning() << "PatientDebitGui::SetupDBus Interface connection not valid";
        DisplayModelessMessageBox("La connexion du client dbus a échoué", true, 5000, QMessageBox::Critical);
    }
}

void PatientDebitGui::Configure() {
    emit nj1_.SIGNAL_Enable();

    // if a dossier has previously been loaded, reload it so that an eventual change
    // in topdejour is taken into account
    if (DossierIsLoaded()) {
        LoadPatient(seance_config_.dossier());
    }
 }

void PatientDebitGui::ConfigureLabJack() {

    try {
        TurnOffConnectedButton();
        dev_factory_->Setup(hardware_);

        QObject::disconnect(dev_factory_->GetMotor(Axis::X).get(), &IMotor::CurrentPosition, this, 0);
        QObject::disconnect(dev_factory_->GetMotor(Axis::Y).get(), &IMotor::CurrentPosition, this, 0);
        QObject::disconnect(dev_factory_->GetMotor(Axis::Z).get(), &IMotor::CurrentPosition, this, 0);
        QObject::disconnect(dev_factory_->GetMotor(Axis::X).get(), &IMotor::MovementFinished, this, 0);
        QObject::disconnect(dev_factory_->GetMotor(Axis::Y).get(), &IMotor::MovementFinished, this, 0);
        QObject::disconnect(dev_factory_->GetMotor(Axis::Z).get(), &IMotor::MovementFinished, this, 0);

        QObject::connect(dev_factory_->GetMotor(Axis::X).get(), &IMotor::CurrentPosition, this, [&](double pos) { parent_->ui()->debit_lineEdit_cuve_pos_x->setText(QString::number(pos, 'f', 2)); });
        QObject::connect(dev_factory_->GetMotor(Axis::Y).get(), &IMotor::CurrentPosition, this, [&](double pos) { parent_->ui()->debit_lineEdit_cuve_pos_y->setText(QString::number(pos, 'f', 2)); });
        QObject::connect(dev_factory_->GetMotor(Axis::Z).get(), &IMotor::CurrentPosition, this, [&](double pos) { parent_->ui()->debit_lineEdit_cuve_pos_z->setText(QString::number(pos, 'f', 2)); });

        QObject::connect(dev_factory_->GetMotor(Axis::X).get(), &IMotor::MovementFinished, this, [&]() { VerifyAxisCalibration(Axis::X); });
        QObject::connect(dev_factory_->GetMotor(Axis::Y).get(), &IMotor::MovementFinished, this, [&]() { VerifyAxisCalibration(Axis::Y); });
        QObject::connect(dev_factory_->GetMotor(Axis::Z).get(), &IMotor::MovementFinished, this, [&]() { VerifyAxisCalibration(Axis::Z); });

        QObject::disconnect(parent_->ui()->debit_pushButton_manual_control_x, 0, 0, 0);
        QObject::disconnect(parent_->ui()->debit_pushButton_manual_control_y, 0, 0, 0);
        QObject::disconnect(parent_->ui()->debit_pushButton_manual_control_z, 0, 0, 0);

        QObject::connect(parent_->ui()->debit_pushButton_manual_control_x, &QPushButton::clicked, this, [=](){ LaunchManualMotorControlDialog(Axis::X, dev_factory_->GetMotor(Axis::X)); });
        QObject::connect(parent_->ui()->debit_pushButton_manual_control_y, &QPushButton::clicked, this, [=](){ LaunchManualMotorControlDialog(Axis::Y, dev_factory_->GetMotor(Axis::Y)); });
        QObject::connect(parent_->ui()->debit_pushButton_manual_control_z, &QPushButton::clicked, this, [=](){ LaunchManualMotorControlDialog(Axis::Z, dev_factory_->GetMotor(Axis::Z)); });

        dev_factory_->CalibrateAxisPosition(Axis::X);
        dev_factory_->CalibrateAxisPosition(Axis::Y);
        dev_factory_->CalibrateAxisPosition(Axis::Z);
        TurnOnConnectedButton();
    } catch (std::exception& exc) {
        QString msg = QString("Failed configuring hardware: ") + exc.what();
        DisplayCritical(msg);
    }
}

void PatientDebitGui::CleanUp() {
    try {
        nj1_.SIGNAL_Disable();
        dev_factory_->CleanUp();
    }
    catch(std::exception& exc) {
        qDebug() << "PatientDebitGui::CleanUp Exception caught " << exc.what();
        DisplayError(QString("Failed cleaning up hardware: ") + exc.what());
    }
}

void PatientDebitGui::MoveToZHalf() {
    try {
        if (!VerifyAxesCalibrations()) {
            return;
        }

        dev_factory_->GetMotor(Axis::X)->SetFastSpeed();
        dev_factory_->GetMotor(Axis::Y)->SetFastSpeed();
        dev_factory_->GetMotor(Axis::Z)->SetFastSpeed();

        const double y_offset = dosimetry_.GetChambre().yoffset();
        const double z_offset = dosimetry_.GetChambre().zoffset();

        qDebug() << "PatientDebitGui::MoveToZHalf Chambre " << dosimetry_.GetChambre().name()
                 << " Yoffset " << dosimetry_.GetChambre().yoffset()
                 << " Zoffset " << dosimetry_.GetChambre().zoffset();

        dev_factory_->GetMotor(Axis::X)->MoveToPosition(0.0);
        dev_factory_->GetMotor(Axis::Y)->MoveToPosition(y_offset);
        dev_factory_->GetMotor(Axis::Z)->MoveToPosition(parent_->ui()->debit_doubleSpinBox_zhalf->value() + z_offset);
    }
    catch (std::exception& exc) {
        qDebug() << "PatientDebitGui::MoveToZHalf Exception thrown " << QString::fromStdString(exc.what());
        DisplayError("Failed moving to Zhalf position:\n\n" + QString::fromStdString(exc.what()));
    }
}


int PatientDebitGui::GetDossier() const {
    return static_cast<int>(seance_config_.dossier());
}

bool PatientDebitGui::DossierIsLoaded() const {
    return seance_config_.Initialised();
}

void PatientDebitGui::VerifyAxisCalibration(Axis axis) {
    try {
        double diff;
        bool ok = dev_factory_->VerifyAxisPositionCalibration(axis, diff);
        emit SIGNAL_AxisDeviation(axis, diff);
        if (!ok) {
            QString message = AxisToString(axis) + "-axis has a larger position deviation than allowed. Consider recalibrating axis.\n";
            message.append("\ndelta = ").append(QString::number(diff, 'f', 2));
            DisplayModelessMessageBox(message, false, 0, QMessageBox::Warning);
        }
        qDebug() << "PatientDebitGui::VerifyAxisCalibration " << AxisToString(axis) << " delta = " << diff;
    } catch (std::exception& exc) {
        QString msg = QString("Failed verifying axis calibrations: ") + exc.what();
        DisplayCritical(msg);
        return;
    }
}

bool PatientDebitGui::VerifyAxesCalibrations() {
    try {
        double x_diff, y_diff, z_diff;
        bool x_ok = dev_factory_->VerifyAxisPositionCalibration(Axis::X, x_diff);
        bool y_ok = dev_factory_->VerifyAxisPositionCalibration(Axis::Y, y_diff);
        bool z_ok = dev_factory_->VerifyAxisPositionCalibration(Axis::Y, z_diff);
        emit SIGNAL_AxisDeviation(Axis::X, x_diff);
        emit SIGNAL_AxisDeviation(Axis::Y, y_diff);
        emit SIGNAL_AxisDeviation(Axis::Z, z_diff);

        if (x_ok && y_ok && z_ok) {
            return true;
        } else {
            QString message = "One or several axes has a larger position deviation than allowed. Consider recalibrating axes.\n";
            message.append("\ndX = ").append(QString::number(x_diff, 'f', 2))
                    .append("\ndY = ").append(QString::number(y_diff, 'f', 2))
                    .append("\ndZ = ").append(QString::number(z_diff, 'f', 2));
            QMessageBox msgBox;
            msgBox.setText(message);
            msgBox.setInformativeText("Continue anyway?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::No);
            return (msgBox.exec() == QMessageBox::Yes);
        }
    } catch (std::exception& exc) {
        QString msg = QString("Failed verifying axes calibrations: ") + exc.what();
        DisplayCritical(msg);
        return false;
    }
}



void PatientDebitGui::UpdateDeliveryConfig() {
    try {
        if (!DossierIsLoaded()) {
            QMessageBox::warning(parent_, "MQA", "Un dossier n'a pas été chargé");
            return;
        }
        SeanceParameterSelectorDialog dialog(parent_, seance_config_);
        if (dialog.exec() == QDialog::Accepted) {
            qDebug() << "PatientDebitGui::ShowChangeSeanceParameterDialog Updating seance config";
            seance_config_ = dialog.GetSeanceConfig();
            WriteDeliveryParameters();
        } else {
            qDebug() << "PatientDebitGui::ShowVhangeSeanceParameterDialog Rejected changes";
        }
    }
    catch (std::exception& exc) {
        qWarning() << QString("PatientDebitGui::UpdateDeliveryConfig Excpetion caught: ") + exc.what();
    }
}

void PatientDebitGui::LaunchManualMotorControlDialog(Axis axis, std::shared_ptr<IMotor> motor) {
    ManualMotorControlDialog dialog(nullptr, axis, motor);
    dialog.exec();
}

void PatientDebitGui::ConnectSignals() {
    // user actions
    QObject::connect(parent_->ui()->debit_pushButton_connected, &QPushButton::clicked, this, &PatientDebitGui::ConfigureLabJack);
    connect(parent_->ui()->debit_lineEdit_temperature, SIGNAL(editingFinished()), this, SLOT(TemperatureChanged()));
    connect(parent_->ui()->debit_lineEdit_pression, SIGNAL(editingFinished()), this, SLOT(PressureChanged()));
    connect(parent_->ui()->debit_comboBox_chambre, SIGNAL(currentIndexChanged(QString)), this, SLOT(ChambreChanged(QString)));
    connect(parent_->ui()->debit_tableWidget_current->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(RemoveMeasurement(int)));
    connect(parent_->ui()->debit_pushButton_save, SIGNAL(clicked()), this, SLOT(SaveCurrentDosimetry()));
    connect(parent_->ui()->debit_pushButton_delete, SIGNAL(clicked()), this, SLOT(RemoveAllMeasurements()));
    connect(parent_->ui()->debit_pushButton_dosiSettingsMore, SIGNAL(clicked()), this, SLOT(LaunchDosimetrySettingsDialog()));

    QObject::connect(this, &PatientDebitGui::SIGNAL_AxisDeviation, this, [&](Axis axis, double value) {
        if (axis == Axis::X) parent_->ui()->debit_lineEdit_cuve_diff_x->setText(QString::number(value, 'f', 2));
        if (axis == Axis::Y) parent_->ui()->debit_lineEdit_cuve_diff_y->setText(QString::number(value, 'f', 2));
        if (axis == Axis::Z) parent_->ui()->debit_lineEdit_cuve_diff_z->setText(QString::number(value, 'f', 2));
    });

    QObject::connect(parent_->ui()->debit_pushButton_movetozhalf,  &QPushButton::clicked, this, &PatientDebitGui::MoveToZHalf);

    // From NJ1
//    QObject::connect(&nj1_, &nj1::NJ1::SIGNAL_Connected, this, &MTD::TurnOnNJ1Button);
//    QObject::connect(&nj1_, &nj1::NJ1::SIGNAL_Connected, this, [&]() {
//        DisplayModelessMessageBox("Connexion aux équipements établi", true, 4, QMessageBox::Information);
//    });
//    QObject::connect(&nj1_, &nj1::NJ1::SIGNAL_Disconnected, this, &MTD::TurnOffNJ1Button);
//    QObject::connect(&nj1_, &nj1::NJ1::SIGNAL_Disconnected, this, [&]() {
//        DisplayModelessMessageBox("Connexion aux équipements rompu", true, 4, QMessageBox::Critical);
//    });

    connect(&nj1_, &nj1::NJ1::SIGNAL_SeancePacket, this, [&](const SeancePacket& seance_packet) {
        parent_->ui()->debit_progressBar_irradiation->setValue(seance_packet.um1());
    });
    connect(&nj1_, &nj1::NJ1::SIGNAL_SeanceFinishedPacket, this, [&](const SeancePacket& seance_packet) {
        parent_->ui()->debit_progressBar_irradiation->setValue(seance_packet.um1());
    });

    // From air ionization chamber
    QObject::connect(radiationmonitor_interface_, &medicyc::cyclotron::RadiationMonitorInterface::SIGNAL_IntegratedChargeOverInterval, this, &PatientDebitGui::SaveAirICCharge);
}

void PatientDebitGui::SetupLineEditValidators() {
    parent_->ui()->debit_lineEdit_temperature->setValidator(new QRegExpValidator(QRegExp("\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->debit_lineEdit_temperature));
    parent_->ui()->debit_lineEdit_pression->setValidator(new QRegExpValidator(QRegExp("\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->debit_lineEdit_pression));
}

void PatientDebitGui::SetupStateMachine() {
    static bool sm_init = false;
    if (sm_init) {
        qWarning() << "PatientDebitGui::SetupStateMachine Already initialized";
        return;
    } else {
        sm_init = true;
    }

    // States
    QState *sSuperState = new QState();
    QState *sIdle = new QState(sSuperState);
    utils::TimedState *sUploadDeliveryConfig = new utils::TimedState(sSuperState, 3000, "N'a pas reçu d'accusé de réception de la configuration de séance", sIdle);
    QState *sLoadPatient = new QState(sSuperState);
    QState *sUpdateDeliveryConfig = new QState(sSuperState);
    QState *sAcknowledgeStarted = new QState(sSuperState);
    QState *sDelivery = new QState(sSuperState);
    QState *sAcknowledgeFinished = new QState(sSuperState);
    QState *sPostProcessResults = new QState(sSuperState);
        utils::TimedState *sGetAirICCharge = new utils::TimedState(sPostProcessResults, 3000, "Echec de la mesure de la chambre d'ionisation de l'air");
        utils::TimedState *sWait = new utils::TimedState(sPostProcessResults, 3000, "", sGetAirICCharge);
        QState *sGetICCharge = new QState(sPostProcessResults);
        QState *sUpdateResults = new QState(sPostProcessResults);

    // SuperState
    sSuperState->setInitialState(sIdle);

    // Idle
    QObject::connect(sIdle, &QState::entered, this, [&]() { QObject::connect(&nj1_, &nj1::NJ1::SIGNAL_SeanceStartedPacket, this, &PatientDebitGui::SeanceStartedReceived); });
    QObject::connect(sIdle, &QState::exited, this, [&]() { QObject::disconnect(&nj1_, &nj1::NJ1::SIGNAL_SeanceStartedPacket, this, &PatientDebitGui::SeanceStartedReceived); });
    sIdle->addTransition(this, &PatientDebitGui::SIGNAL_SeanceStarted, sAcknowledgeStarted);
    sIdle->addTransition(parent_->ui()->debit_pushButton_upload_delivery_config, &QPushButton::clicked, sUploadDeliveryConfig);
    sIdle->addTransition(parent_->ui()->debit_pushButton_update_delivery_config, &QPushButton::clicked, sUpdateDeliveryConfig);
    sIdle->addTransition(parent_->ui()->debit_pushButton_load_patient, &QPushButton::clicked, sLoadPatient);

    // UploadDeliveryConfig
    QObject::connect(sUploadDeliveryConfig, &QState::entered, this, &PatientDebitGui::UploadDeliveryConfig);
    QObject::connect(sUploadDeliveryConfig, &utils::TimedState::SIGNAL_Timeout, this, [&](QString message) { DisplayModelessMessageBox(message, true, 60, QMessageBox::Critical); });
    sUploadDeliveryConfig->addTransition(this, &PatientDebitGui::SIGNAL_Abort, sIdle);
    auto tConfigAckOk = sUploadDeliveryConfig->addTransition(&nj1_, &nj1::NJ1::SIGNAL_SeanceConfigAckOk, sIdle);
    QObject::connect(tConfigAckOk, &QSignalTransition::triggered, this, [&]() { DisplayModelessMessageBox("Un accusé de réception de la configuration de la séance a été reçu.", true, 1, QMessageBox::Information); });
    auto tConfigAckError = sUploadDeliveryConfig->addTransition(&nj1_, &nj1::NJ1::SIGNAL_SeanceConfigAckError, sIdle);
    QObject::connect(tConfigAckError, &QSignalTransition::triggered, this, [&]() { DisplayModelessMessageBox("Non-concordance entre la configuration de séance envoyée et reçue", true, 60, QMessageBox::Critical); });

    // UpdateDeliveryParameters
    QObject::connect(sUpdateDeliveryConfig, &QState::entered, this, &PatientDebitGui::UpdateDeliveryConfig);
    sUpdateDeliveryConfig->addTransition(sUpdateDeliveryConfig, &QState::entered, sIdle);

    // LoadPatient
    QObject::connect(sLoadPatient, &QState::entered, this, &PatientDebitGui::ChoosePatient);
    sLoadPatient->addTransition(sLoadPatient, &QState::entered, sIdle);

    // AcknowledgeStarted
    QObject::connect(sAcknowledgeStarted, &QState::entered, radiationmonitor_interface_, &medicyc::cyclotron::RadiationMonitorInterface::StartChargeMeasurement);
    QObject::connect(sAcknowledgeStarted, &QState::entered, &nj1_, &nj1::NJ1::AcknowledgeSeanceStart);
    sAcknowledgeStarted->addTransition(sAcknowledgeStarted, &QState::entered, sDelivery);

    // Delivery
    QObject::connect(sDelivery, &QState::entered, this, [&]() { QObject::connect(&nj1_, &nj1::NJ1::SIGNAL_SeancePacket, this, &PatientDebitGui::SeancePacketReceived); });
    QObject::connect(sDelivery, &QState::exited, this, [&]() { QObject::disconnect(&nj1_, &nj1::NJ1::SIGNAL_SeancePacket, this, &PatientDebitGui::SeancePacketReceived); });
    QObject::connect(sDelivery, &QState::entered, this, [&]() { QObject::connect(&nj1_, &nj1::NJ1::SIGNAL_SeanceFinishedPacket, this, &PatientDebitGui::SeanceFinishedReceived); });
    QObject::connect(sDelivery, &QState::exited, this, [&]() { QObject::disconnect(&nj1_, &nj1::NJ1::SIGNAL_SeanceFinishedPacket, this, &PatientDebitGui::SeanceFinishedReceived); });
    sDelivery->addTransition(this, &PatientDebitGui::SIGNAL_SeanceFinished, sAcknowledgeFinished);
    auto tDeliveryTimeout = sDelivery->addTransition(&nj1_, &nj1::NJ1::SIGNAL_SeancePacketTimeout, sAcknowledgeFinished);
    QObject::connect(tDeliveryTimeout, &QSignalTransition::triggered, this, [&]() {
        DisplayModelessMessageBox("Connexion interrompue pendant la livraison du traitement. La dose enregistrée peut être erronée.", true, 600, QMessageBox::Critical); });

    // AcknowledgeFinished
    QObject::connect(sAcknowledgeFinished, &QState::entered, &nj1_, &nj1::NJ1::AcknowledgeSeanceFinish);
    sAcknowledgeFinished->addTransition(sAcknowledgeFinished, &QState::entered, sPostProcessResults);

    // PostProcessResults
    sPostProcessResults->setInitialState(sWait);

    // Wait
    // No further actions

    // GetAirICCharge
    QObject::connect(sGetAirICCharge, &QState::entered, this, &PatientDebitGui::ClearAirICCharge);
    QObject::connect(sGetAirICCharge, &QState::entered, radiationmonitor_interface_, &medicyc::cyclotron::RadiationMonitorInterface::StopChargeMeasurement);
    QObject::connect(sGetAirICCharge, &utils::TimedState::SIGNAL_Timeout, this, [&](QString message) {
        DisplayModelessMessageBox(message, true, 5, QMessageBox::Warning); });
    sGetAirICCharge->addTransition(radiationmonitor_interface_, &medicyc::cyclotron::RadiationMonitorInterface::SIGNAL_IntegratedChargeOverInterval, sGetICCharge);
    sGetAirICCharge->addTransition(sGetAirICCharge, &utils::TimedState::SIGNAL_Timeout, sGetICCharge);

    // GetICCharge
    QObject::connect(sGetICCharge, &QState::entered, this, [&]() { ic_charge_ = 0.0; });
    QObject::connect(sGetICCharge, &QState::entered, this, &PatientDebitGui::GetICCharge);
    sGetICCharge->addTransition(this, &PatientDebitGui::SIGNAL_ICChargeGiven, sUpdateResults);
    sGetICCharge->addTransition(this, &PatientDebitGui::SIGNAL_Abort, sIdle);

    // UpdateResults
    QObject::connect(sUpdateResults, &QState::entered, this, &PatientDebitGui::UpdateResults);
    sUpdateResults->addTransition(sUpdateResults, &QState::entered, sIdle);

    PrintStateChanges(sSuperState, "SuperState");
    PrintStateChanges(sIdle, "Idle");
    PrintStateChanges(sUploadDeliveryConfig, "UploadDeliveryConfig");
    PrintStateChanges(sLoadPatient, "LoadPatient");
    PrintStateChanges(sUpdateDeliveryConfig, "UpdateDeliveryConfig");
    PrintStateChanges(sAcknowledgeStarted, "AcknowledgeStarted");
    PrintStateChanges(sDelivery, "Delivery");
    PrintStateChanges(sAcknowledgeFinished, "AcknowledgeFinished");
    PrintStateChanges(sPostProcessResults, "PostProcessResults");
    PrintStateChanges(sWait, "Wait");
    PrintStateChanges(sGetAirICCharge, "GetAirICCharge");
    PrintStateChanges(sGetICCharge, "GetICCharge");
    PrintStateChanges(sUpdateResults, "UpdateResults");

    sm_.addState(sSuperState);
    sm_.setInitialState(sSuperState);
    sm_.start();
}

void PatientDebitGui::SeanceStartedReceived(const SeancePacket& packet) {
    seance_packets_.clear();
    seance_packets_.push_back(packet);
    emit (SIGNAL_SeanceStarted());
}

void PatientDebitGui::SeancePacketReceived(const SeancePacket& packet) {
    seance_packets_.push_back(packet);
}

void PatientDebitGui::SeanceFinishedReceived(const SeancePacket& packet) {
    seance_packets_.push_back(packet);
    emit (SIGNAL_SeanceFinished());
}

void PatientDebitGui::SeanceTimeoutReceived() {
    emit (SIGNAL_SeanceTimeout());
    QMessageBox::critical(parent_, "Débit Patient", "La transmission de données de la séance a été interrompu. La dose enregistrée ne sera pas correct!");
}

void PatientDebitGui::UploadDeliveryConfig() {
    if (!DossierIsLoaded()) {
        DisplayModelessMessageBox("Un dossier n'a pas été chargé", true, 5, QMessageBox::Warning);
        emit SIGNAL_Abort();
        return;
    }
    if (!nj1_.IsConnected()) {
        DisplayModelessMessageBox("Pas de connexion a l'automate", true, 5, QMessageBox::Warning);
        emit SIGNAL_Abort();
        return;
    }
    nj1_.UploadNewSeanceConfig(seance_config_);
    parent_->ui()->debit_progressBar_irradiation->setValue(0);
    parent_->ui()->debit_progressBar_irradiation->setMaximum(seance_config_.mu_des());
}

void PatientDebitGui::GetICCharge() {
    DoseRateMeasurementDialog dosimetryMeasurementDialog;
    dosimetryMeasurementDialog.show();
    if (dosimetryMeasurementDialog.exec() == QDialog::Accepted) {
        ic_charge_ = dosimetryMeasurementDialog.GetCharge();
        emit (SIGNAL_ICChargeGiven());
    } else {
        ic_charge_ = 0.0;
        emit (SIGNAL_Abort());
    }
}

void PatientDebitGui::SaveAirICCharge(double charge) {
    air_ic_charge_ = std::max(charge, 0.0);
}

void PatientDebitGui::ClearAirICCharge() {
    air_ic_charge_ = 0.0;
}

void PatientDebitGui::UpdateResults() {
    try {
        SeanceRecord record = seance_config_.Convert(QDateTime::currentDateTime(), seance_packets_);
        DosimetryRecord dos_record(record.GetDuration(),
                                   record.GetIChambre1(),
                                   record.GetIChambre2(),
                                   record.GetIStripper(),
                                   record.GetICF9(),
                                   record.GetUMPrevu(),
                                   record.GetUM1Delivered(),
                                   ic_charge_,
                                   air_ic_charge_);
        dosimetry_.AddRecord(dos_record);
        WriteResults();
    }
    catch(std::exception& exc) {
        qWarning() << QString("PatientDebitGui::UpdateResults Exception caught") << exc.what();
        QMessageBox::warning(parent_, "MQA", "Echec de l'ajout de mesure");
    }
}

void PatientDebitGui::ChoosePatient() {
    try {
        DossierSelectionDialog dialog(parent_, pt_repo_.get());
        if (dialog.exec() == QDialog::Accepted) {
            const int dossier = dialog.dossier();
            qDebug() << "PatientDebitGui::ChoosePatient Accepted " << dossier;
            LoadPatient(dossier);
        }
    }
    catch (std::exception& exc) {
        ClearData();
        qWarning() << "PatientDebitGui::ChoosePatient Exception thrown: " << QString::fromStdString(exc.what());
        DisplayError("Un problème est survenu:\n\n" + QString::fromStdString(exc.what()));
    }

}

void PatientDebitGui::LoadPatient(int dossier) {
    try {
        ClearData();
        Treatment treatment = pt_repo_->GetTreatment(dossier);
        parent_->ui()->debit_label_id->setText(QString::number(dossier));
        parent_->ui()->debit_label_nom->setText(treatment.GetPatient().GetLastName().toUpper());
        seance_config_ = GetSeanceConfig(treatment);
        WriteDeliveryParameters();
        FillSavedDosimetryTable();
    }
    catch (std::exception& exc) {
        ClearData();
        qWarning() << "PatientDebitGui::LoadDossier Exception thrown: " << QString::fromStdString(exc.what());
        DisplayError("Un problème est survenu:\n\n" + QString::fromStdString(exc.what()));
    }
}

// can throw
void PatientDebitGui::FillSavedDosimetryTable() {
    parent_->ui()->debit_tableWidget_saved_doserates->clearContents();
    QDate today = QDate::currentDate();
    std::vector<Debit> debits = pt_repo_->GetDebits(GetDossier(), today.addYears(-1), today);
    int row = static_cast<int>(debits.size()) - 1;
    for (const Debit& d : debits) {
        QString debit = (d.monte_carlo() ? QString::number(d.mean(), 'f', 4) : AdjustForTop(d.timestamp(), d.mean()));
        QString type = (d.monte_carlo() ? QString("Calculé") : QString("Mesuré"));
        parent_->ui()->debit_tableWidget_saved_doserates->setItem(row, (int)SAVED_GRIDCOLS::TIMESTAMP,
                                                                  new AlignedQTableWidgetItem(d.timestamp().toString("dd/MM hh:mm"), Qt::AlignHCenter));
        parent_->ui()->debit_tableWidget_saved_doserates->setItem(row, (int)SAVED_GRIDCOLS::TYPE,
                                                                  new AlignedQTableWidgetItem(type, Qt::AlignHCenter));
        parent_->ui()->debit_tableWidget_saved_doserates->setItem(row, (int)SAVED_GRIDCOLS::DEBIT,
                                                                  new AlignedQTableWidgetItem(debit, Qt::AlignHCenter));
        --row;
    }
}

QString PatientDebitGui::AdjustForTop(QDateTime timestamp, double debit) const {
    try {
        Dosimetry ref_tdj = pt_repo_->GetReferenceDosimetry(timestamp);
        double adjusted = (debit * pt_repo_->GetDefaults().GetDRef() / ref_tdj.GetDebitMean());
        return QString::number(adjusted, 'f', 4);
    }
    catch (std::exception& exc) {
        qWarning() << "PatientDebitGui::AdjustForTop Exception: " << exc.what();
        return QString("No top");
    }
}

// can throw
double PatientDebitGui::GetEstimatedDebit(int dossier) const {
    auto mod_ref(pt_repo_->GetModulateurForDossier(10000));
    auto mod_act(pt_repo_->GetModulateurForDossier(dossier));
    auto deg_set_ref(pt_repo_->GetDegradeurSetForDossier(10000));
    auto deg_set_act(pt_repo_->GetDegradeurSetForDossier(dossier));

    auto defaults = pt_repo_->GetDefaults();
    auto mcnpx_data = pt_repo_->GetMCNPXData();
    SOBPMaker sobpMaker(mcnpx_data.depth_dose_curves(),
                        mcnpx_data.monitor_units(),
                        mcnpx_data.um_plexi());
    auto sobp_ref = sobpMaker.GetSOBP(mod_ref, deg_set_ref);
    auto sobp_act = sobpMaker.GetSOBP(mod_act, deg_set_act);
    if (calc::AlmostEqual(sobp_ref.DMUZHalf(), 0.0, 0.0001) ||
            calc::AlmostEqual(sobp_act.DMUZHalf(), 0.0, 0.0001) ||
            calc::AlmostEqual(defaults.GetDRef(), 0.0, 0.0001)) {
        throw std::runtime_error("One debit is zero");
    }
    const double debit_mcnpx(defaults.GetDRef() * sobp_act.DMUZHalf() / sobp_ref.DMUZHalf());
    return debit_mcnpx;
}


// can throw
SeanceConfig PatientDebitGui::GetSeanceConfig(Treatment &treatment) const {
    const double debit = GetEstimatedDebit(treatment.GetDossier());
    if (!treatment.GetActiveSeance()) {
        throw std::runtime_error("Il n'y plus de séances prévues pour ce dossier. Ajouter un et réessayer");
    }
    const double dose =  treatment.GetActiveSeance()->GetDosePrescribed();
    const int um = static_cast<int>(std::round(calc::DoseToUM(debit, dose)));
    double duration = 0.0;
    double i_stripper = 0.0;
    util::GetStripperCurrentAndSeanceDuration(debit, dose, 10.0, &i_stripper, &duration);

    return SeanceConfig(treatment.GetPatient().GetLastName(),
                        treatment.GetDossier(),
                        1, // seance number (irrelevant in this case)
                        static_cast<float>(dose),
                        static_cast<float>(debit),
                        static_cast<uint32_t>(um),
                        static_cast<uint32_t>(0), // um corr (no longer used)
                        static_cast<uint32_t>(duration * 1000),
                        static_cast<uint32_t>(i_stripper));
}

void PatientDebitGui::CheckForUnSavedChanges() {
    if (dosimetry_.UnSavedChanges()) {
        QMessageBox msgBox;
        msgBox.setText("Il y a des mesures non sauvegardées.");
        msgBox.setInformativeText("Voulez-vous enregistrer vos modifications?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        switch (ret) {
        case QMessageBox::Yes:
            this->SaveCurrentDosimetry();
            break;
        case QMessageBox::No:
            break;
        default:
            qWarning() << "PatientDebitGui::CheckForUnSavedChanges";
            break;
        }
    }
}

void PatientDebitGui::SaveCurrentDosimetry() {
    this->SaveDosimetry(dosimetry_);
}

void PatientDebitGui::SaveDosimetry(Dosimetry& dosimetry) {
    if (!DossierIsLoaded()) {
        QMessageBox::warning(parent_, "MQA", "Un dossier n'a pas été chargé");
        return;
    }
    if (!dosimetry.HasMeasurements()) {
        QMessageBox::information(parent_, "MQA", "Aucune mesure n'a été faite");
        return;
    }
    if (!dosimetry.MeasurementConditionsEntered()) {
         QMessageBox::information(parent_, "MQA", "La température, la pression ou la chambre n'ont pas été saisies");
         return;
    }

    QDateTime timestamp(QDateTime::currentDateTime());
    dosimetry.SetTimestamp(timestamp);
    dosimetry.SetMonteCarlo(false);
    dosimetry.SetReference(false);
    try {
        pt_repo_->SaveDosimetry(GetDossier(), dosimetry);
        dosimetry.HasBeenSaved();
        FillSavedDosimetryTable();
        QMessageBox::information(parent_, "MQA", "La mesure a été enregistrée dans la base de données");
    }
    catch (std::exception& exc) {
        QMessageBox::warning(parent_, "MQA", QString("Une erreur s'est produite lors de l'enregistrement de la mesure: ") + exc.what());
        qWarning() << QString("There was an error saving the dosimetry to db: ") + exc.what();
    }
}

void PatientDebitGui::RemoveAllMeasurements() {
    QMessageBox msgBox;
    msgBox.setText("Voulez-vous vraiment supprimer toutes les mesures?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Yes:
        dosimetry_.DeleteRecords();
        WriteResults();
        break;
    case QMessageBox::No:
        break;
    default:
        qWarning() << "PatientDebitGui::RemoveAllMeasurements Not covered case";
        break;
    }
}

void PatientDebitGui::RemoveMeasurement(int row) {
    if (row < (int)dosimetry_.GetRecords().size()) {
        QMessageBox msgBox;
        msgBox.setText("Voulez-vous vraiment supprimer cette mesure?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        switch (ret) {
        case QMessageBox::Yes:
            dosimetry_.DeleteSingleRecord(row);
            WriteResults();
            break;
        case QMessageBox::No:
            break;
        default:
            qWarning() << "PatientDebitGui::RemoveMeasurement Not covered case";
            break;
        }
    }
}

void PatientDebitGui::InitTables() {
    QStringList headerLabels_current;
    headerLabels_current << "UM\ndélivrée" << "I.Chambre1\n[nA]" << "I.Chambre2\n[nA]"
                         << "Charge\n[nC]" << "Charge Amb.\n[pC]" << "Débit\n[cGy/UM]" << "D.Moyen\n[cGy/UM]";
    parent_->ui()->debit_tableWidget_current->setHorizontalHeaderLabels(headerLabels_current);
    parent_->ui()->debit_tableWidget_current->scrollToTop();

    QStringList headerLabels_saved;
    headerLabels_saved << "Timestamp" << "Type" << "Débit\n[cGy/UM]\nREF Cond";
    parent_->ui()->debit_tableWidget_saved_doserates->setHorizontalHeaderLabels(headerLabels_saved);
    parent_->ui()->debit_tableWidget_saved_doserates->scrollToTop();
    parent_->ui()->debit_tableWidget_current->setColumnWidth((int)CURRENT_GRIDCOLS::DMOYEN, 140);
    parent_->ui()->debit_tableWidget_current->setColumnWidth((int)CURRENT_GRIDCOLS::UM, 90);
    parent_->ui()->debit_tableWidget_current->setColumnWidth((int)CURRENT_GRIDCOLS::DEBIT, 90);
    parent_->ui()->debit_tableWidget_current->setColumnWidth((int)CURRENT_GRIDCOLS::ICHAMBRE1, 90);
    parent_->ui()->debit_tableWidget_current->setColumnWidth((int)CURRENT_GRIDCOLS::ICHAMBRE2, 90);

}

void PatientDebitGui::InitProgressBar() {
    parent_->ui()->debit_progressBar_irradiation->setMinimum(0);
    parent_->ui()->debit_progressBar_irradiation->setMaximum(1000);
    parent_->ui()->debit_progressBar_irradiation->setValue(0);
    parent_->ui()->debit_progressBar_irradiation->setStyleSheet(QString::fromUtf8("text-align: center;"));
}

void PatientDebitGui::TemperatureChanged() {
    // validator used to guarante that text can be casted to double
    double temperature(parent_->ui()->debit_lineEdit_temperature->text().toDouble());
    dosimetry_.SetTemperature(temperature);
    if (dosimetry_.HasMeasurements()) WriteResults();
    if (!dosimetry_.TemperatureInRange(2.0)) {
        QMessageBox::warning(parent_, "MQA", "La température donnée est >2 deviations standard en dehors de la valeur moyenne");
    }
}

void PatientDebitGui::PressureChanged() {
    // validator used to guarante that text can be casted to double
    dosimetry_.SetPressure(parent_->ui()->debit_lineEdit_pression->text().toDouble());
    if (dosimetry_.HasMeasurements()) WriteResults();
    if (!dosimetry_.PressureInRange(2.0)) {
        QMessageBox::warning(parent_, "MQA", "La pression atmosphérique donnée est >2 deviations standard en dehors de la valeur moyenne");
    }
}

void PatientDebitGui::ChambreChanged(const QString& chambre) {
    try {
        const bool first_time_choice = !dosimetry_.ChambreEntered();
        dosimetry_.SetChambre(pt_repo_->GetChambre(chambre));
        if (dosimetry_.HasMeasurements()) WriteResults();

        // If this is the first time the user selects a chambe,
        // remove the "Sélectionner" item from the list of chambres in
        // the combobox
        if (first_time_choice) {
            RemoveSelectorChambre();
        }
    }
    catch (std::exception& exc) {
        QMessageBox::warning(parent_, "Database error", "Failed retreiving chambre: " + chambre);
        qWarning() << QString("PatientDebitGui::ChambreChanged Failed retrieving chambre from db: ") + exc.what();
    }
}

void PatientDebitGui::ClearData() {
    seance_packets_.clear();
    dosimetry_.DeleteRecords();

    parent_->ui()->debit_label_id->setText("NA");
    parent_->ui()->debit_label_nom->setText("NA");
    parent_->ui()->debit_label_um->setText("NA");
    parent_->ui()->debit_label_duration->setText("NA");
    parent_->ui()->debit_label_current_stripper->setText("NA");

//    parent_->ui()->debit_lineEdit_temperature->clear();
//    parent_->ui()->debit_lineEdit_pression->clear();
//    parent_->ui()->debit_comboBox_chambre->clear();

    parent_->ui()->debit_tableWidget_current->clearContents();
    parent_->ui()->debit_tableWidget_saved_doserates->clearContents();
}

void PatientDebitGui::InitChambres() {
    try {
        QStringList chambres = {chambre_list_default_text_};
        chambres.append(pt_repo_->GetChambres());
        parent_->ui()->debit_comboBox_chambre->addItems(chambres);
        int index = parent_->ui()->debit_comboBox_chambre->findText(chambre_list_default_text_);
        parent_->ui()->debit_comboBox_chambre->setCurrentIndex(index);
    }
    catch (std::exception& exc) {
        QMessageBox::information(parent_, "MQA", exc.what());
        qWarning() << "PatientDebitGui::Init Failed retrieving chambres from db";
    }
}

void PatientDebitGui::RemoveSelectorChambre() {
     try {
        int index = parent_->ui()->debit_comboBox_chambre->findText(chambre_list_default_text_);
        parent_->ui()->debit_comboBox_chambre->removeItem(index);
     }
     catch (std::exception& exc) {
        QMessageBox::information(parent_, "MQA", exc.what());
        qWarning() << "PatientDebitGui::RemoveSelectorChambre Failed retrieving chambres from db";
     }
}


void PatientDebitGui::WriteResults() {
    parent_->ui()->debit_tableWidget_current->clearContents();
    auto records = dosimetry_.GetRecords();
    auto debits = dosimetry_.GetDebits();
    auto rollingDebitMeans = dosimetry_.GetRollingDebitMeans();
    auto rollingDebitStdDevs = dosimetry_.GetRollingDebitStdDevs();
    int row(0);
    for (auto record : records) {
        parent_->ui()->debit_tableWidget_current->setItem(row, (int)CURRENT_GRIDCOLS::UM, new AlignedQTableWidgetItem(QString::number(record.GetTotalUMDelivered()), Qt::AlignHCenter));
        parent_->ui()->debit_tableWidget_current->setItem(row, (int)CURRENT_GRIDCOLS::ICHAMBRE1, new AlignedQTableWidgetItem(QString::number(calc::Mean(record.GetIChambre1()), 'f', 2), Qt::AlignHCenter));
        parent_->ui()->debit_tableWidget_current->setItem(row, (int)CURRENT_GRIDCOLS::ICHAMBRE2, new AlignedQTableWidgetItem(QString::number(calc::Mean(record.GetIChambre2()), 'f', 2), Qt::AlignHCenter));
        parent_->ui()->debit_tableWidget_current->setItem(row, (int)CURRENT_GRIDCOLS::IC_CHARGE, new AlignedQTableWidgetItem(QString::number(record.GetCharge(), 'f', 4), Qt::AlignHCenter));
        parent_->ui()->debit_tableWidget_current->setItem(row, (int)CURRENT_GRIDCOLS::AIR_IC_CHARGE, new AlignedQTableWidgetItem(QString::number(1.0E12 * record.GetAirICCharge(), 'f', 4), Qt::AlignHCenter));
        parent_->ui()->debit_tableWidget_current->setItem(row, (int)CURRENT_GRIDCOLS::DEBIT, new AlignedQTableWidgetItem(dosimetry_.MeasurementConditionsEntered() ? QString::number(debits.at(row), 'f', 4) : "NA", Qt::AlignHCenter));
        QString debit_with_stddev(QString::number(rollingDebitMeans.at(row), 'f', 4) + " \u00B1 "  + QString::number(rollingDebitStdDevs.at(row), 'f', 4));
        parent_->ui()->debit_tableWidget_current->setItem(row, (int)CURRENT_GRIDCOLS::DMOYEN, new AlignedQTableWidgetItem(dosimetry_.MeasurementConditionsEntered() ? debit_with_stddev : "NA", Qt::AlignHCenter));
        row++;
    }
}

void PatientDebitGui::LaunchDosimetrySettingsDialog() {
   DoseRateSettingsDialog dosimetrySettingsDialog(dosimetry_.GetChambre());
   dosimetrySettingsDialog.exec();
}

void PatientDebitGui::WriteDeliveryParameters() {
    parent_->ui()->debit_label_um->setText(QString::number(seance_config_.mu_des()));
    parent_->ui()->debit_label_duration->setText(QString::number(static_cast<double>(seance_config_.duration()) / 1000.0, 'f', 1));
    parent_->ui()->debit_label_current_stripper->setText(QString::number(seance_config_.current_stripper()));
}

void PatientDebitGui::PrintStateChanges(QAbstractState *state, QString name) {
    QObject::connect(state, &QState::entered, this, [&, name]() { qDebug() << "PatientDebit " << QDateTime::currentDateTime() << " ->" << name; });
    QObject::connect(state, &QState::exited, this, [&, name]() { qDebug() << "PatientDebit " << QDateTime::currentDateTime() << " <-" << name; });
}

