#include "TopDeJourGui.h"

#include <cstdlib>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <set>
#include <QMessageBox>
#include <QStandardPaths>
#include "unistd.h"

#include "DevFactory.h"
#include "ui_MQA.h"
#include "ManualMotorControlDialog.h"
#include "MQA.h"
#include "Chambre.h"
#include "DoseRateMeasurementDialog.h"
#include "DoseRateSettingsDialog.h"
#include "Modulateur.h"
#include "Degradeur.h"
#include "Util.h"
#include "Calc.h"
#include "SeanceParameterSelectorDialog.h"
#include "TimedState.h"

TopDeJourGui::TopDeJourGui(MQA *parent, std::shared_ptr<PTRepo> pt_repo, std::shared_ptr<QARepo> qa_repo) :
    GuiBase(parent),
    settings_(std::unique_ptr<QSettings>(new QSettings(QStandardPaths::locate(QStandardPaths::ConfigLocation, QString("MQA.ini"), QStandardPaths::LocateFile),
                                                          QSettings::IniFormat))),
    pt_repo_(pt_repo), qa_repo_(qa_repo),
    nj1_("MQA", false)
{
    dev_factory_ = std::shared_ptr<DevFactory>(new DevFactory(qa_repo_));
    InitTables();
    InitDateSelectors();
    SetupGraphs();
    SetupDBus();
    FillAcquisitionPageGraphs();
    InitChambres();
    ConnectSignals();
    SetupLineEditValidators();
    SetupStateMachine();

    try {
        SetDefaultMeasurementConfig();
        //SetDefaultMeasurementConditions();
    }
    catch (std::exception& exc) {
        qWarning() << QString("TopDeJourGui::TopDeJourGui Exception thrown: ") + exc.what();
    }
}

TopDeJourGui::~TopDeJourGui() {
    if (radiationmonitor_interface_) {
        delete radiationmonitor_interface_;
    }
    TurnOffConnectedButton();
}

void TopDeJourGui::SetupDBus() {
    QString dbus_host = settings_->value("dbus/host", "").toString();
    QString dbus_port = settings_->value("dbus/port", "").toString();
    QString dbus_name = settings_->value("dbus/name", "").toString();
    QString dbus_conn = QString("tcp:host=%1,port=%2").arg(dbus_host).arg(dbus_port);
    qDebug() << "TopDeJourGui::SetupDBus "
             << "Host " << dbus_host
             << "Port " << dbus_port
             << "Name " << dbus_name
             << "Full connection name " << dbus_conn;
    QDBusConnection dbus_connection = QDBusConnection::connectToBus(dbus_conn, dbus_name);
    if (dbus_connection.isConnected()) {
        qDebug() << "TopDeJourGui::SetupDBus DBus connection established";
    } else {
        qWarning() << "TopDeJourGui::SetupDBus DBus connection failed";
        DisplayModelessMessageBox("La connexion au bus dbus a échoué", true, 5000, QMessageBox::Critical);
    }
    radiationmonitor_interface_ = new medicyc::cyclotron::RadiationMonitorInterface("medicyc.cyclotron.hardware.radiation_monitor.RadiationMonitor_Salle_Clinic",
                                                                                    "/RadiationMonitor_Salle_Clinic", dbus_connection, this);
    if (radiationmonitor_interface_->isValid()) {
        qDebug() << "TopDeJourGui::SetupDBus Interface connection valid";
    } else {
        qWarning() << "TopDeJourGui::SetupDBus Interface connection not valid";
        DisplayModelessMessageBox("La connexion du client dbus a échoué", true, 5000, QMessageBox::Critical);
    }
}

void TopDeJourGui::SetDefaultMeasurementConfig () {
    try {
        const QString name = "TDJ";
        const uint32_t dossier = 10000;
        const uint16_t seance_number = 1;
        const float debit = 1.37;
        const uint32_t mu = 1000;
        const float dose = calc::UMToDose(debit, mu);
        const uint32_t current_stripper = 70;
        const uint32_t duration = static_cast<uint32_t>(1000.0 * util::GetSeanceDuration(debit, dose, static_cast<double>(current_stripper)));

        seance_config_ = SeanceConfig(name, dossier, seance_number, dose, debit,
                                      mu, 0, duration, current_stripper);
        WriteDeliveryParameters();
    }
    catch (std::exception& exc) {
        seance_config_ = SeanceConfig();
        qWarning() << "TopDeJourGui::InitConfig Exception caught: " << exc.what();
        QMessageBox::warning(parent_, tr("MQA"), tr("Impossible d'extraire les données de la base de données"));
    }
}

void TopDeJourGui::SetupGraphs() {
    debit_graph_ = std::shared_ptr<DebitGraph>(new DebitGraph(parent_->ui()->tdj_customPlot_historical_doserate));


    parent_->ui()->tdj_progressBar_irradiation->setMinimum(0);
    parent_->ui()->tdj_progressBar_irradiation->setMaximum(1000);
    parent_->ui()->tdj_progressBar_irradiation->setValue(0);
    parent_->ui()->tdj_progressBar_irradiation->setStyleSheet(QString::fromUtf8("text-align: center;"));
}

void TopDeJourGui::TurnOffConnectedButton() {
    parent_->ui()->tdj_pushButton_connected->setStyleSheet(ButtonStyleSheet("connected_red_button.png"));
}

void TopDeJourGui::TurnOnConnectedButton() {
    parent_->ui()->tdj_pushButton_connected->setStyleSheet(ButtonStyleSheet("connected_green_button.png"));
}

void TopDeJourGui::FillAcquisitionPageGraphs() {
    try {
        QDate from_date(parent_->ui()->tdj_dateEdit_from->date());
        QDate to_date(parent_->ui()->tdj_dateEdit_to->date());

        debit_graph_->RemoveAll();
        std::vector<Debit> all_debits = pt_repo_->GetDebits(dossier_number_, from_date, to_date);
        std::set<QString> unique_chambres;
        for (const Debit& debit : all_debits) {
            unique_chambres.insert(debit.chambre().name());
        }

        std::map<QString, std::vector<double>> value_chambre;
        std::map<QString, std::vector<Debit>> debit_chambre;
        for (const QString& ch : unique_chambres) {
            value_chambre[ch] = std::vector<double>();
            debit_chambre[ch] = std::vector<Debit>();
        }

        for (const Debit& d : all_debits) {
            value_chambre[d.chambre().name()].push_back(d.mean());
            debit_chambre[d.chambre().name()].push_back(d);
        }

        for (const QString& ch : unique_chambres) {
            debit_graph_->Register(ch);
            debit_graph_->SetData(ch, debit_chambre.at(ch));
        }
    }
    catch(std::exception& exc) {
        qWarning() << "TopDeJourGui::FillGraphs Exception thrown: " << exc.what();
    }
}

void TopDeJourGui::LaunchManualMotorControlDialog(Axis axis, std::shared_ptr<IMotor> motor) {
    ManualMotorControlDialog dialog(nullptr, axis, motor);
    dialog.exec();
}

void TopDeJourGui::Configure() {
    emit nj1_.SIGNAL_Enable();
}

void TopDeJourGui::ConfigureLabJack() {
    try {
        TurnOffConnectedButton();
        dev_factory_->Setup(hardware_);

        QObject::disconnect(dev_factory_->GetMotor(Axis::X).get(), &IMotor::CurrentPosition, this, 0);
        QObject::disconnect(dev_factory_->GetMotor(Axis::Y).get(), &IMotor::CurrentPosition, this, 0);
        QObject::disconnect(dev_factory_->GetMotor(Axis::Z).get(), &IMotor::CurrentPosition, this, 0);
        QObject::disconnect(dev_factory_->GetMotor(Axis::X).get(), &IMotor::MovementFinished, this, 0);
        QObject::disconnect(dev_factory_->GetMotor(Axis::Y).get(), &IMotor::MovementFinished, this, 0);
        QObject::disconnect(dev_factory_->GetMotor(Axis::Z).get(), &IMotor::MovementFinished, this, 0);

        QObject::connect(dev_factory_->GetMotor(Axis::X).get(), &IMotor::CurrentPosition, this, [&](double pos) { parent_->ui()->tdj_lineEdit_cuve_pos_x->setText(QString::number(pos, 'f', 2)); });
        QObject::connect(dev_factory_->GetMotor(Axis::Y).get(), &IMotor::CurrentPosition, this, [&](double pos) { parent_->ui()->tdj_lineEdit_cuve_pos_y->setText(QString::number(pos, 'f', 2)); });
        QObject::connect(dev_factory_->GetMotor(Axis::Z).get(), &IMotor::CurrentPosition, this, [&](double pos) { parent_->ui()->tdj_lineEdit_cuve_pos_z->setText(QString::number(pos, 'f', 2)); });

        QObject::connect(dev_factory_->GetMotor(Axis::X).get(), &IMotor::MovementFinished, this, [&]() { VerifyAxisCalibration(Axis::X); });
        QObject::connect(dev_factory_->GetMotor(Axis::Y).get(), &IMotor::MovementFinished, this, [&]() { VerifyAxisCalibration(Axis::Y); });
        QObject::connect(dev_factory_->GetMotor(Axis::Z).get(), &IMotor::MovementFinished, this, [&]() { VerifyAxisCalibration(Axis::Z); });

        QObject::disconnect(parent_->ui()->tdj_pushButton_manual_control_x, 0, 0, 0);
        QObject::disconnect(parent_->ui()->tdj_pushButton_manual_control_y, 0, 0, 0);
        QObject::disconnect(parent_->ui()->tdj_pushButton_manual_control_z, 0, 0, 0);

        QObject::connect(parent_->ui()->tdj_pushButton_manual_control_x, &QPushButton::clicked, this, [=](){ LaunchManualMotorControlDialog(Axis::X, dev_factory_->GetMotor(Axis::X)); });
        QObject::connect(parent_->ui()->tdj_pushButton_manual_control_y, &QPushButton::clicked, this, [=](){ LaunchManualMotorControlDialog(Axis::Y, dev_factory_->GetMotor(Axis::Y)); });
        QObject::connect(parent_->ui()->tdj_pushButton_manual_control_z, &QPushButton::clicked, this, [=](){ LaunchManualMotorControlDialog(Axis::Z, dev_factory_->GetMotor(Axis::Z)); });

        dev_factory_->CalibrateAxisPosition(Axis::X);
        dev_factory_->CalibrateAxisPosition(Axis::Y);
        dev_factory_->CalibrateAxisPosition(Axis::Z);
        TurnOnConnectedButton();
    } catch (std::exception& exc) {
        QString msg = QString("Failed configuring hardware: ") + exc.what();
        DisplayCritical(msg);
    }
}

void TopDeJourGui::VerifyAxisCalibration(Axis axis) {
    try {
        double diff;
        bool ok = dev_factory_->VerifyAxisPositionCalibration(axis, diff);
        emit SIGNAL_AxisDeviation(axis, diff);
        if (!ok) {
            QString message = AxisToString(axis) + "-axis has a larger position deviation than allowed. Consider recalibrating axis.\n";
            message.append("\ndelta = ").append(QString::number(diff, 'f', 2));
            DisplayModelessMessageBox(message, false, 0, QMessageBox::Warning);
        }
        qDebug() << "TopDeJourGui::VerifyAxisCalibration " << AxisToString(axis) << " delta = " << diff;
    } catch (std::exception& exc) {
        QString msg = QString("Failed verifying axis calibrations: ") + exc.what();
        DisplayCritical(msg);
        return;
    }
}

bool TopDeJourGui::VerifyAxesCalibrations() {
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

void TopDeJourGui::UpdateDeliveryConfig() {
    try {
        SeanceParameterSelectorDialog dialog(parent_, seance_config_);
        if (dialog.exec() == QDialog::Accepted) {
            qDebug() << "TopDeJourGui::UpdateDeliveryConfig Updating seance config";
            seance_config_ = dialog.GetSeanceConfig();
            WriteDeliveryParameters();
        } else {
            qDebug() << "TopDeJourGui::UpdateDeliveryConfig Rejected changes";
        }
    } catch (std::exception& exc) {
        qWarning() << QString("TopDeJour::UpdateDeliveryConfig Excpetion caught: ") + exc.what();
    }
}

void TopDeJourGui::CleanUp() {
    try {
        emit nj1_.SIGNAL_Disable();
        dev_factory_->CleanUp();
    }
    catch(std::exception& exc) {
        qDebug() << "TopDeJourGui::CleanUp Exception caught " << exc.what();
        DisplayError(QString("Failed cleaning up hardware:\n\n ") + exc.what());
    }
}

void TopDeJourGui::MoveToZHalf() {
    try {
        if (!VerifyAxesCalibrations()) {
            return;
        }

        dev_factory_->GetMotor(Axis::X)->SetFastSpeed();
        dev_factory_->GetMotor(Axis::Y)->SetFastSpeed();
        dev_factory_->GetMotor(Axis::Z)->SetFastSpeed();

        const double y_offset = dosimetry_.GetChambre().yoffset();
        const double z_offset = dosimetry_.GetChambre().zoffset();

        qDebug() << "TopDeJourGui::MoveToZHalf Chambre " << dosimetry_.GetChambre().name()
                 << " Yoffset " << dosimetry_.GetChambre().yoffset()
                 << " Zoffset " << dosimetry_.GetChambre().zoffset();

        dev_factory_->GetMotor(Axis::X)->MoveToPosition(0.0);
        dev_factory_->GetMotor(Axis::Y)->MoveToPosition(y_offset);
        dev_factory_->GetMotor(Axis::Z)->MoveToPosition(parent_->ui()->tdj_doubleSpinBox_zhalf->value() + z_offset);
    }
    catch (std::exception& exc) {
        qDebug() << "TopDeJourGui::MoveToZHalf Exception thrown " << QString::fromStdString(exc.what());
        DisplayError("Failed moving to Zhalf position:\n\n " + QString::fromStdString(exc.what()));
    }
}

void TopDeJourGui::ConnectSignals() {
    // user actions
    QObject::connect(parent_->ui()->tdj_pushButton_connected, &QPushButton::clicked, this, &TopDeJourGui::ConfigureLabJack);
    connect(parent_->ui()->tdj_pushButton_results_update, SIGNAL(clicked()), this, SLOT(FillAcquisitionPageGraphs()));
    connect(parent_->ui()->tdj_lineEdit_temperature, SIGNAL(editingFinished()), this, SLOT(TemperatureChanged()));
    connect(parent_->ui()->tdj_lineEdit_pression, SIGNAL(editingFinished()), this, SLOT(PressureChanged()));
    connect(parent_->ui()->tdj_comboBox_chambre, SIGNAL(currentIndexChanged(QString)), this, SLOT(ChambreChanged(QString)));
    connect(parent_->ui()->tdj_tableWidget_current->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(RemoveMeasurement(int)));
    connect(parent_->ui()->tdj_pushButton_save, SIGNAL(clicked()), this, SLOT(SaveCurrentDosimetry()));
    connect(parent_->ui()->tdj_pushButton_delete, SIGNAL(clicked()), this, SLOT(RemoveAllMeasurements()));
    connect(parent_->ui()->tdj_pushButton_dosiSettingsMore, SIGNAL(clicked()), this, SLOT(LaunchDosimetrySettingsDialog()));
    QObject::connect(parent_->ui()->tdj_pushButton_movetozhalf,  &QPushButton::clicked, this, &TopDeJourGui::MoveToZHalf);

    QObject::connect(this, &TopDeJourGui::SIGNAL_AxisDeviation, this, [&](Axis axis, double value) {
        if (axis == Axis::X) parent_->ui()->tdj_lineEdit_cuve_diff_x->setText(QString::number(value, 'f', 2));
        if (axis == Axis::Y) parent_->ui()->tdj_lineEdit_cuve_diff_y->setText(QString::number(value, 'f', 2));
        if (axis == Axis::Z) parent_->ui()->tdj_lineEdit_cuve_diff_z->setText(QString::number(value, 'f', 2));
    });

    connect(&nj1_, &nj1::NJ1::SIGNAL_SeancePacket, this, [&](const SeancePacket& seance_packet) {
        parent_->ui()->tdj_progressBar_irradiation->setValue(seance_packet.um1());
    });
    connect(&nj1_, &nj1::NJ1::SIGNAL_SeanceFinishedPacket, this, [&](const SeancePacket& seance_packet) {
        parent_->ui()->tdj_progressBar_irradiation->setValue(seance_packet.um1());
    });


    // From NJ1
//    QObject::connect(&nj1_, &nj1::NJ1::SIGNAL_Connected, this, &MTD::TurnOnNJ1Button);
//    QObject::connect(&nj1_, &nj1::NJ1::SIGNAL_Connected, this, [&]() {
//        DisplayModelessMessageBox("Connexion aux équipements établi", true, 4, QMessageBox::Information);
//    });
//    QObject::connect(&nj1_, &nj1::NJ1::SIGNAL_Disconnected, this, &MTD::TurnOffNJ1Button);
//    QObject::connect(&nj1_, &nj1::NJ1::SIGNAL_Disconnected, this, [&]() {
//        DisplayModelessMessageBox("Connexion aux équipements rompu", true, 4, QMessageBox::Critical);
//    });

    // From air ionization chamber
    QObject::connect(radiationmonitor_interface_, &medicyc::cyclotron::RadiationMonitorInterface::SIGNAL_IntegratedChargeOverInterval, this, &TopDeJourGui::SaveAirICCharge);

    // tabwidget
    QObject::connect(parent_->ui()->tdj_tabWidget, &QTabWidget::currentChanged, this, &TopDeJourGui::TabWidgetPageChanged);
}

void TopDeJourGui::SetupLineEditValidators() {
    parent_->ui()->tdj_lineEdit_temperature->setValidator(new QRegExpValidator(QRegExp("\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->tdj_lineEdit_temperature));
    parent_->ui()->tdj_lineEdit_pression->setValidator(new QRegExpValidator(QRegExp("\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->tdj_lineEdit_pression));
}

void TopDeJourGui::SetupStateMachine() {
    static bool sm_init = false;
    if (sm_init) {
        qWarning() << "TopDeJourGui::SetupStateMachine Already initialized";
        return;
    } else {
        sm_init = true;
    }

    // States
    QState *sSuperState = new QState();
    QState *sIdle = new QState(sSuperState);
    utils::TimedState *sUploadDeliveryConfig = new utils::TimedState(sSuperState, 3000, "N'a pas reçu d'accusé de réception de la configuration de séance", sIdle);
    QState *sUpdateDeliveryConfig = new QState(sSuperState);
    QState *sAcknowledgeStarted = new QState(sSuperState);
    QState *sDelivery = new QState(sSuperState);
    QState *sAcknowledgeFinished = new QState(sSuperState);
    QState *sPostProcessResults = new QState(sSuperState);
        utils::TimedState *sGetAirICCharge = new utils::TimedState(sPostProcessResults, 3000, "Echec de la mesure de la chambre d'ionisation de l'air");
        utils::TimedState *sWait = new utils::TimedState(sPostProcessResults, 3000, "Wait", sGetAirICCharge);
        QState *sGetICCharge = new QState(sPostProcessResults);
        QState *sUpdateResults = new QState(sPostProcessResults);

         // SuperState
        sSuperState->setInitialState(sIdle);

        // Idle
        QObject::connect(sIdle, &QState::entered, this, [&]() { QObject::connect(&nj1_, &nj1::NJ1::SIGNAL_SeanceStartedPacket, this, &TopDeJourGui::SeanceStartedReceived); });
        QObject::connect(sIdle, &QState::exited, this, [&]() { QObject::disconnect(&nj1_, &nj1::NJ1::SIGNAL_SeanceStartedPacket, this, &TopDeJourGui::SeanceStartedReceived); });
        sIdle->addTransition(this, &TopDeJourGui::SIGNAL_SeanceStarted, sAcknowledgeStarted);
        sIdle->addTransition(parent_->ui()->tdj_pushButton_upload_delivery_config, &QPushButton::clicked, sUploadDeliveryConfig);
        sIdle->addTransition(parent_->ui()->tdj_pushButton_update_delivery_config, &QPushButton::clicked, sUpdateDeliveryConfig);

        // UploadDeliveryConfig
        QObject::connect(sUploadDeliveryConfig, &QState::entered, this, &TopDeJourGui::UploadDeliveryConfig);
        QObject::connect(sUploadDeliveryConfig, &utils::TimedState::SIGNAL_Timeout, this, [&](QString message) { DisplayModelessMessageBox(message, true, 60, QMessageBox::Critical); });
        sUploadDeliveryConfig->addTransition(this, &TopDeJourGui::SIGNAL_Abort, sIdle);
        auto tConfigAckOk = sUploadDeliveryConfig->addTransition(&nj1_, &nj1::NJ1::SIGNAL_SeanceConfigAckOk, sIdle);
        QObject::connect(tConfigAckOk, &QSignalTransition::triggered, this, [&]() { DisplayModelessMessageBox("Un accusé de réception de la configuration de la séance a été reçu.", true, 1, QMessageBox::Information); });
        auto tConfigAckError = sUploadDeliveryConfig->addTransition(&nj1_, &nj1::NJ1::SIGNAL_SeanceConfigAckError, sIdle);
        QObject::connect(tConfigAckError, &QSignalTransition::triggered, this, [&]() { DisplayModelessMessageBox("Non-concordance entre la configuration de séance envoyée et reçue", true, 60, QMessageBox::Critical); });

        // UpdateDeliveryParameters
        QObject::connect(sUpdateDeliveryConfig, &QState::entered, this, &TopDeJourGui::UpdateDeliveryConfig);
        sUpdateDeliveryConfig->addTransition(sUpdateDeliveryConfig, &QState::entered, sIdle);

        // AcknowledgeStarted
        QObject::connect(sAcknowledgeStarted, &QState::entered, radiationmonitor_interface_, &medicyc::cyclotron::RadiationMonitorInterface::StartChargeMeasurement);
        QObject::connect(sAcknowledgeStarted, &QState::entered, &nj1_, &nj1::NJ1::AcknowledgeSeanceStart);
        sAcknowledgeStarted->addTransition(sAcknowledgeStarted, &QState::entered, sDelivery);

        // Delivery
        QObject::connect(sDelivery, &QState::entered, this, [&]() { QObject::connect(&nj1_, &nj1::NJ1::SIGNAL_SeancePacket, this, &TopDeJourGui::SeancePacketReceived); });
        QObject::connect(sDelivery, &QState::exited, this, [&]() { QObject::disconnect(&nj1_, &nj1::NJ1::SIGNAL_SeancePacket, this, &TopDeJourGui::SeancePacketReceived); });
        QObject::connect(sDelivery, &QState::entered, this, [&]() { QObject::connect(&nj1_, &nj1::NJ1::SIGNAL_SeanceFinishedPacket, this, &TopDeJourGui::SeanceFinishedReceived); });
        QObject::connect(sDelivery, &QState::exited, this, [&]() { QObject::disconnect(&nj1_, &nj1::NJ1::SIGNAL_SeanceFinishedPacket, this, &TopDeJourGui::SeanceFinishedReceived); });
        sDelivery->addTransition(this, &TopDeJourGui::SIGNAL_SeanceFinished, sAcknowledgeFinished);
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
        QObject::connect(sGetAirICCharge, &utils::TimedState::SIGNAL_Timeout, this, [&](QString message) {
            DisplayModelessMessageBox(message, true, 5, QMessageBox::Warning); });
        QObject::connect(sGetAirICCharge, &QState::entered, this, &TopDeJourGui::ClearAirICCharge);
        QObject::connect(sGetAirICCharge, &QState::entered, radiationmonitor_interface_, &medicyc::cyclotron::RadiationMonitorInterface::StopChargeMeasurement);
        sGetAirICCharge->addTransition(radiationmonitor_interface_, &medicyc::cyclotron::RadiationMonitorInterface::SIGNAL_IntegratedChargeOverInterval, sGetICCharge);
        sGetAirICCharge->addTransition(sGetAirICCharge, &utils::TimedState::SIGNAL_Timeout, sGetICCharge);

        // GetICCharge
        QObject::connect(sGetICCharge, &QState::entered, this, [&]() { ic_charge_ = 0.0; });
        QObject::connect(sGetICCharge, &QState::entered, this, &TopDeJourGui::GetICCharge);
        sGetICCharge->addTransition(this, &TopDeJourGui::SIGNAL_ICChargeGiven, sUpdateResults);
        sGetICCharge->addTransition(this, &TopDeJourGui::SIGNAL_Abort, sIdle);

        // UpdateResults
        QObject::connect(sUpdateResults, &QState::entered, this, &TopDeJourGui::UpdateResults);
        sUpdateResults->addTransition(sUpdateResults, &QState::entered, sIdle);


        PrintStateChanges(sSuperState, "SuperState");
        PrintStateChanges(sIdle, "Idle");
        PrintStateChanges(sUploadDeliveryConfig, "UploadDeliveryConfig");
        PrintStateChanges(sUpdateDeliveryConfig, "UpdateDeliveryConfig");
        PrintStateChanges(sAcknowledgeStarted, "AcknowledgeStarted");
        PrintStateChanges(sDelivery, "Delivery");
        PrintStateChanges(sAcknowledgeFinished, "AcknowledgeFinished");
        PrintStateChanges(sWait, "Wait");
        PrintStateChanges(sPostProcessResults, "PostProcessResults");
        PrintStateChanges(sGetAirICCharge, "GetAirICCharge");
        PrintStateChanges(sGetICCharge, "GetICCharge");
        PrintStateChanges(sUpdateResults, "UpdateResults");

    sm_.addState(sSuperState);
    sm_.setInitialState(sSuperState);
    sm_.start();
}

void TopDeJourGui::SeanceStartedReceived(const SeancePacket& packet) {
    seance_packets_.clear();
    seance_packets_.push_back(packet);
    emit SIGNAL_SeanceStarted();
}

void TopDeJourGui::SeancePacketReceived(const SeancePacket& packet) {
    seance_packets_.push_back(packet);
}

void TopDeJourGui::SeanceFinishedReceived(const SeancePacket& packet) {
    seance_packets_.push_back(packet);
    emit SIGNAL_SeanceFinished();
}

void TopDeJourGui::SeanceTimeoutReceived() {
    emit SIGNAL_SeanceTimeout();
    QMessageBox::critical(parent_, "TopDuJour", "La transmission de données de la séance a été interrompu. La dose enregistrée ne sera pas correct!");
}

void TopDeJourGui::GetICCharge() {
    DoseRateMeasurementDialog dosimetryMeasurementDialog;
    dosimetryMeasurementDialog.show();
    if (dosimetryMeasurementDialog.exec() == QDialog::Accepted) {
        ic_charge_ = dosimetryMeasurementDialog.GetCharge();
	qDebug() << "TopDeJourGui::GetICCharge " << ic_charge_;
        emit (SIGNAL_ICChargeGiven());
    } else {
        ic_charge_ = 0.0;
        emit (SIGNAL_Abort());
    }
}

void TopDeJourGui::SaveAirICCharge(double charge) {
    air_ic_charge_ = std::max(0.0, charge);
}

void TopDeJourGui::ClearAirICCharge() {
    air_ic_charge_ = 0.0;
}

void TopDeJourGui::UpdateResults() {
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
        qDebug() << "TopDeJourGui::UpdateResults UM delivered " << record.GetTotalUMDelivered() << " IC charge " << ic_charge_;
        dosimetry_.AddRecord(dos_record);
        WriteResults();
    }
    catch(std::exception& exc) {
        qWarning() << QString("TopDeJour::UpdateResults Exception caught") << exc.what();
        QMessageBox::warning(parent_, "MQA", "Echec de l'ajout de mesure");
    }
    emit SIGNAL_UpdateResultsDone();
}

void TopDeJourGui::CheckForUnSavedChanges() {
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
            qWarning() << "TopDeJourGui::CheckForUnSavedChanges";
            break;
        }
    }
}

void TopDeJourGui::SaveCurrentDosimetry() {
    this->SaveDosimetry(dosimetry_);
}

void TopDeJourGui::SaveDosimetry(Dosimetry& dosimetry) {
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
    dosimetry.SetReference(true); // TDJ
    try {
        pt_repo_->SaveDosimetry(dossier_number_, dosimetry);
        dosimetry.HasBeenSaved();
        FillAcquisitionPageGraphs();
        QMessageBox::information(parent_, "MQA", "La mesure a été enregistrée dans la base de données");
    }
    catch (std::exception& exc) {
        QMessageBox::warning(parent_, "MQA", QString("Une erreur s'est produite lors de l'enregistrement de la mesure: ") + exc.what());
        qWarning() << QString("There was an error saving the dosimetry to db: ") + exc.what();
    }
}

void TopDeJourGui::RemoveAllMeasurements() {
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
        qWarning() << "TopDeJourGui::RemoveAllMeasurements Not covered case";
        break;
    }
}

void TopDeJourGui::RemoveMeasurement(int row) {
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
            qWarning() << "TopDeJourGui::RemoveMeasurement Not covered case";
            break;
        }
    }
}

void TopDeJourGui::InitTables() {
    QStringList headerLabels_current;
    headerLabels_current << "UM\ndélivrée" << "I.Chambre1\n[nA]" << "Transmission\n[%]"
                         << "Charge\n[nC]" << "Charge Amb.\n[pC]" << "Débit\n[cGy/UM]" << "D.Moyen\n[cGy/UM]";
    parent_->ui()->tdj_tableWidget_current->setHorizontalHeaderLabels(headerLabels_current);
    parent_->ui()->tdj_tableWidget_current->scrollToTop();
    parent_->ui()->tdj_tableWidget_current->setColumnWidth((int)CURRENT_GRIDCOLS::DMOYEN, 140);
}

void TopDeJourGui::InitDateSelectors() {
    parent_->ui()->tdj_dateEdit_from->setDisplayFormat("yyyy.MM.dd");
    parent_->ui()->tdj_dateEdit_to->setDisplayFormat("yyyy.MM.dd");
    parent_->ui()->tdj_dateEdit_from->setCurrentSection(QDateTimeEdit::MonthSection);
    parent_->ui()->tdj_dateEdit_to->setCurrentSection(QDateTimeEdit::MonthSection);
    QDateTime current_date(QDateTime::currentDateTime());
    parent_->ui()->tdj_dateEdit_from->setDate(current_date.date().addDays(-365));
    parent_->ui()->tdj_dateEdit_to->setDate(current_date.date());
}

void TopDeJourGui::TemperatureChanged() {
    // validator used to guarante that text can be casted to double
    double temperature(parent_->ui()->tdj_lineEdit_temperature->text().toDouble());
    dosimetry_.SetTemperature(temperature);
    if (dosimetry_.HasMeasurements()) WriteResults();
    if (!dosimetry_.TemperatureInRange(2.0)) {
        QMessageBox::warning(parent_, "MQA", "La température donnée est >2 deviations standard en dehors de la valeur moyenne");
    }
}

void TopDeJourGui::PressureChanged() {
    // validator used to guarante that text can be casted to double
    dosimetry_.SetPressure(parent_->ui()->tdj_lineEdit_pression->text().toDouble());
   if (dosimetry_.HasMeasurements()) WriteResults();
   if (!dosimetry_.PressureInRange(2.0)) {
       QMessageBox::warning(parent_, "MQA", "La pression atmosphérique donnée est >2 deviations standard en dehors de la valeur moyenne");
   }
}

void TopDeJourGui::ChambreChanged(const QString& chambre) {
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
        QMessageBox::warning(parent_, "MQA", "N'a pas trouvé chambre: " + chambre);
        qWarning() << QString("TopDeJourGui::ChambreChanged Failed retrieving chambre from db: ") + exc.what();
    }
}

void TopDeJourGui::ClearData() {
    parent_->ui()->tdj_lineEdit_temperature->clear();
    parent_->ui()->tdj_lineEdit_pression->clear();
    parent_->ui()->tdj_comboBox_chambre->clear();
    parent_->ui()->tdj_tableWidget_current->clearContents();
}

void TopDeJourGui::InitChambres() {
    try {
        QStringList chambres = {chambre_list_default_text_};
        chambres.append(pt_repo_->GetChambres());
        parent_->ui()->tdj_comboBox_chambre->addItems(chambres);
        int index = parent_->ui()->tdj_comboBox_chambre->findText(chambre_list_default_text_);
        parent_->ui()->tdj_comboBox_chambre->setCurrentIndex(index);
    }
    catch (std::exception& exc) {
        QMessageBox::information(parent_, "MQA", exc.what());
        qWarning() << "TopDeJourGui::Init Failed retrieving chambres from db";
    }
}

void TopDeJourGui::RemoveSelectorChambre() {
    try {
        int index = parent_->ui()->tdj_comboBox_chambre->findText(chambre_list_default_text_);
        parent_->ui()->tdj_comboBox_chambre->removeItem(index);
    }
    catch (std::exception& exc) {
        QMessageBox::information(parent_, "MQA", exc.what());
        qWarning() << "TopDeJourGui::RemoveSelectorChambre Failed retrieving chambres from db";
    }
}

void TopDeJourGui::WriteResults() {
    parent_->ui()->tdj_tableWidget_current->clearContents();
    auto records = dosimetry_.GetRecords();
    auto debits = dosimetry_.GetDebits();
    auto rollingDebitMeans = dosimetry_.GetRollingDebitMeans();
    auto rollingDebitStdDevs = dosimetry_.GetRollingDebitStdDevs();
    Chambre c = dosimetry_.GetChambre();
    qDebug() << "TopDeJourGui::WriteResults Chambre " << c.id() << " " << c.name() << " " << c.kqq0() << " " << c.nd();
    int row(0);
    for (DosimetryRecord record : records) {
        parent_->ui()->tdj_tableWidget_current->setItem(row, (int)CURRENT_GRIDCOLS::UM, new AlignedQTableWidgetItem(QString::number(record.GetTotalUMDelivered()), Qt::AlignHCenter));
        parent_->ui()->tdj_tableWidget_current->setItem(row, (int)CURRENT_GRIDCOLS::ICHAMBRE1, new AlignedQTableWidgetItem(QString::number(calc::Mean(record.GetIChambre1()), 'f', 2), Qt::AlignHCenter));
        parent_->ui()->tdj_tableWidget_current->setItem(row, (int)CURRENT_GRIDCOLS::TRANSMISSION, new AlignedQTableWidgetItem(QString::number(record.GetBeamTransmission()), Qt::AlignHCenter));
        parent_->ui()->tdj_tableWidget_current->setItem(row, (int)CURRENT_GRIDCOLS::IC_CHARGE, new AlignedQTableWidgetItem(QString::number(record.GetCharge(), 'f', 4), Qt::AlignHCenter));
        parent_->ui()->tdj_tableWidget_current->setItem(row, (int)CURRENT_GRIDCOLS::AIR_IC_CHARGE, new AlignedQTableWidgetItem(QString::number(1.0E12 * record.GetAirICCharge(), 'f', 4), Qt::AlignHCenter));
        parent_->ui()->tdj_tableWidget_current->setItem(row, (int)CURRENT_GRIDCOLS::DEBIT, new AlignedQTableWidgetItem(dosimetry_.MeasurementConditionsEntered() ? QString::number(debits.at(row), 'f', 4) : "NA", Qt::AlignHCenter));
        QString debit_with_stddev(QString::number(rollingDebitMeans.at(row), 'f', 4) + " \u00B1 "  + QString::number(rollingDebitStdDevs.at(row), 'f', 4));
	qDebug() << "TopDeJourGui::WriteResults Debit " << debit_with_stddev;
        parent_->ui()->tdj_tableWidget_current->setItem(row, (int)CURRENT_GRIDCOLS::DMOYEN, new AlignedQTableWidgetItem(dosimetry_.MeasurementConditionsEntered() ? debit_with_stddev : "NA", Qt::AlignHCenter));
        row++;
    }
}

void TopDeJourGui::LaunchDosimetrySettingsDialog() {
   DoseRateSettingsDialog dosimetrySettingsDialog(dosimetry_.GetChambre());
   dosimetrySettingsDialog.exec();
}

void TopDeJourGui::WriteDeliveryParameters() {
    parent_->ui()->tdj_label_um->setText(QString::number(seance_config_.mu_des()));
    parent_->ui()->tdj_label_duration->setText(QString::number(static_cast<double>(seance_config_.duration()) / 1000.0, 'f', 1));
    parent_->ui()->tdj_label_current_stripper->setText(QString::number(seance_config_.current_stripper()));
}

void TopDeJourGui::UploadDeliveryConfig() {
    if (!nj1_.IsConnected()) {
        emit SIGNAL_Abort();
        QMessageBox::warning(parent_, "MQA", "Pas de connexion a l'automate");
        return;
    }
    nj1_.UploadNewSeanceConfig(seance_config_);
    parent_->ui()->tdj_progressBar_irradiation->setValue(0);
    parent_->ui()->tdj_progressBar_irradiation->setMaximum(seance_config_.mu_des());
}

void TopDeJourGui::PrintStateChanges(QAbstractState *state, QString name) {
    QObject::connect(state, &QState::entered, this, [&, name]() { qDebug() << "TopDeJour " << QDateTime::currentDateTime() << " ->" << name; });
    QObject::connect(state, &QState::exited, this, [&, name]() { qDebug() << "TopDeJour " << QDateTime::currentDateTime() << " <-" << name; });
}

void TopDeJourGui::TabWidgetPageChanged(int index) {

    TABWIDGETPAGE page = static_cast<TABWIDGETPAGE>(index);
    switch (page) {
    case TABWIDGETPAGE::ACQUISITION:
        break;
    case TABWIDGETPAGE::AMBIENTCHARGE:
        break;
    default:
        break;
    }
}

