#include "MTD.h"

#include <QDebug>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>
#include <unistd.h>
#include <stdexcept>
#include <algorithm>

#include "CollimateurDialog.h"
#include "CompensateurTransparantDialog.h"
#include "SeanceParameterSelectorDialog.h"
#include "BaliseCalibration.h"
#include "TimedState.h"

#include "Calc.h"
#include "Algorithms.h"

MTD::MTD(QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::MTD),
    settings_(std::unique_ptr<QSettings>(new QSettings(QStandardPaths::locate(QStandardPaths::ConfigLocation, QString("MTD.ini"), QStandardPaths::LocateFile),
                                                          QSettings::IniFormat))),
    repo_(nullptr),
    nj1_("MTD", true)
{
    ui_->setupUi(this);
    setWindowTitle(QString("Medicyc Treatment Delivery (MTD) v2.0.0"));

    if (QFile(settings_->fileName()).exists()) {
        qDebug() << "MTD.ini found";
    } else {
        qWarning() << "MTD.ini not found";
        DisplayModelessMessageBox("Fichier de configuration introuvable: ~.config/MTD.ini ", true, 5000, QMessageBox::Critical);
        return;
    }

    InitButtons();
    SetupGraphs();
    SetupDBus();
    SetupDb();
    ConnectSignals();
    SetupStateMachine();
    SetupInternalGroupBox();
}

MTD::~MTD() {
    if (repo_) delete repo_;
	this->close();
    if (radiationmonitor_interface_) {
        delete radiationmonitor_interface_;
    }
}

void MTD::InitButtons() {
    TurnOffDbButton();
    TurnOffNJ1Button();
}

void MTD::SetupGraphs() {
    seance_dose_graph_ = std::unique_ptr<SeanceDoseGraph>(new SeanceDoseGraph(ui_->seanceDoseChartView));
    seance_progress_graph_ = std::unique_ptr<SeanceProgressGraph>(new SeanceProgressGraph(ui_->seanceProgressChartView, true, true));

    ui_->genericGauge_um_seance->Configure("Intrafraction", "UM", "CT1", "CT2", 0);
    ui_->genericGauge_dose_seance->Configure("Séance", "Gy", "CT1", "MRM", 2);
    ui_->genericGauge_dose_total->Configure("Cumulé", "Gy", "CT1", "MRM", 2);
}

void MTD::ConnectSignals() {
    // From UI
    QObject::connect(ui_->pushButton_db, &QPushButton::clicked, this, &MTD::SetupDb);
    QObject::connect(ui_->pushButton_configuration, &QPushButton::clicked, this, &MTD::ShowConfiguration);
    QObject::connect(ui_->pushButton_collimateur, &QPushButton::clicked, this, &MTD::ShowCollimator);
    QObject::connect(ui_->pushButton_compensateur, &QPushButton::clicked, this, &MTD::ShowCompensator);
    QObject::connect(seance_dose_graph_.get(), SIGNAL(ShowSeanceInfoReq(int)), this, SLOT(ShowSeanceInfo(int)));

    // From NJ1
    QObject::connect(&nj1_, &nj1::NJ1::SIGNAL_Connected, this, &MTD::TurnOnNJ1Button);
    QObject::connect(&nj1_, &nj1::NJ1::SIGNAL_Connected, this, [&]() {
        DisplayModelessMessageBox("Connexion aux équipements établi", true, 4, QMessageBox::Information);
    });
    QObject::connect(&nj1_, &nj1::NJ1::SIGNAL_Disconnected, this, &MTD::TurnOffNJ1Button);
    QObject::connect(&nj1_, &nj1::NJ1::SIGNAL_Disconnected, this, [&]() {
        DisplayModelessMessageBox("Connexion aux équipements rompu", true, 4, QMessageBox::Critical);
    });
}

void MTD::SetupDBus() {
    if (!(settings_->contains("dbus/host") && settings_->contains("dbus/port") && settings_->contains("dbus/name"))) {
        qWarning() << "MTD.ini does not contain all necessary dbus parameters (host, port, name)";
        DisplayModelessMessageBox("Le fichier de configuration MTD.ini ne contient pas tous les paramètres nécessaires", true, 5000, QMessageBox::Critical);
    }
    QString dbus_host = settings_->value("dbus/host", "").toString();
    QString dbus_port = settings_->value("dbus/port", "").toString();
    dbus_name_ = settings_->value("dbus/name", "").toString();
    QString dbus_conn = QString("tcp:host=%1,port=%2").arg(dbus_host).arg(dbus_port);
    qDebug() << "MTD::SetupDBus "
             << "Host " << dbus_host
             << "Port " << dbus_port
             << "Name " << dbus_name_
             << "Full connection name " << dbus_conn;
    QDBusConnection dbus_connection = QDBusConnection::connectToBus(dbus_conn, dbus_name_);
    if (dbus_connection.isConnected()) {
        qDebug() << "MTD::SetupDBus DBus connection established";
    } else {
        qWarning() << "MTD::SetupDBus DBus connection failed";
        DisplayModelessMessageBox("La connexion au bus dbus a échoué", true, 5000, QMessageBox::Critical);
    }
    radiationmonitor_interface_ = new medicyc::cyclotron::RadiationMonitorInterface("medicyc.cyclotron.hardware.radiation_monitor.RadiationMonitor_Salle_Clinic",
                                                                                    "/RadiationMonitor_Salle_Clinic", dbus_connection, this);
    if (radiationmonitor_interface_->isValid()) {
        qDebug() << "MTD::SetupDBus Interface connection valid";
    } else {
        qWarning() << "MTD::SetupDBus Interface connection not valid";
        DisplayModelessMessageBox("La connexion du client dbus a échoué", true, 5000, QMessageBox::Critical);
    }
    QObject::connect(radiationmonitor_interface_, &medicyc::cyclotron::RadiationMonitorInterface::SIGNAL_IntegratedChargeOverInterval, this, &MTD::CalculateEstimatedDose);
}

void MTD::SetupInternalGroupBox() {
    ui_->groupBox_internal->setVisible(false);
    QObject::connect(ui_->pushButton_show_internal, &QPushButton::clicked, this, [=]() { ui_->groupBox_internal->setVisible(!ui_->groupBox_internal->isVisible()); });
}

void MTD::SetupStateMachine() {
    static bool sm_init = false;
    if (sm_init) {
        qWarning() << "MTD::SetupStateMachine Already initialized";
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
    QState *sIncrementSeance = new QState(sSuperState);
    QState *sAcknowledgeStarted = new QState(sSuperState);
    QState *sDelivery = new QState(sSuperState);
    QState *sAcknowledgeFinished = new QState(sSuperState);
    QState *sPostProcessResults = new QState(sSuperState);
        utils::TimedState *sGetEstimatedDose = new utils::TimedState(sPostProcessResults, 3000, "Echec de la mesure de la chambre d'ionisation de l'air");
        utils::TimedState *sWait = new utils::TimedState(sPostProcessResults, 3000, "Wait", sGetEstimatedDose);
        QState *sUpdateResults = new QState(sPostProcessResults);

    // SuperState
    sSuperState->setInitialState(sIdle);

    // Idle
    QObject::connect(sIdle, &QState::entered, this, [&]() { QObject::connect(&nj1_, &nj1::NJ1::SIGNAL_SeanceStartedPacket, this, &MTD::SeanceStartedReceived); });
    QObject::connect(sIdle, &QState::exited, this, [&]() { QObject::disconnect(&nj1_, &nj1::NJ1::SIGNAL_SeanceStartedPacket, this, &MTD::SeanceStartedReceived); });
    sIdle->addTransition(this, &MTD::SIGNAL_SeanceStarted, sAcknowledgeStarted);
    sIdle->addTransition(ui_->pushButton_upload_delivery_config, &QPushButton::clicked, sUploadDeliveryConfig);
    sIdle->addTransition(ui_->pushButton_update_delivery_config, &QPushButton::clicked, sUpdateDeliveryConfig);
    sIdle->addTransition(ui_->pushButton_load_patient, &QPushButton::clicked, sLoadPatient);
    sIdle->addTransition(ui_->pushButton_pass_to_next_seance, &QPushButton::clicked, sIncrementSeance);

    // UploadDeliveryConfig
    QObject::connect(sUploadDeliveryConfig, &QState::entered, this, &MTD::UploadDeliveryConfig);
    QObject::connect(sUploadDeliveryConfig, &utils::TimedState::SIGNAL_Timeout, this, [&](QString message) { DisplayModelessMessageBox(message, true, 60, QMessageBox::Critical); });
    sUploadDeliveryConfig->addTransition(this, &MTD::SIGNAL_Abort, sIdle);
    auto tConfigAckOk = sUploadDeliveryConfig->addTransition(&nj1_, &nj1::NJ1::SIGNAL_SeanceConfigAckOk, sIdle);
    QObject::connect(tConfigAckOk, &QSignalTransition::triggered, this, [&]() { DisplayModelessMessageBox("Un accusé de réception de la configuration de la séance a été reçu.", true, 1, QMessageBox::Information); });
    auto tConfigAckError = sUploadDeliveryConfig->addTransition(&nj1_, &nj1::NJ1::SIGNAL_SeanceConfigAckError, sIdle);
    QObject::connect(tConfigAckError, &QSignalTransition::triggered, this, [&]() { DisplayModelessMessageBox("Non-concordance entre la configuration de séance envoyée et reçue", true, 60, QMessageBox::Critical); });

    // UpdateDeliveryParameters
    QObject::connect(sUpdateDeliveryConfig, &QState::entered, this, &MTD::UpdateDeliveryConfig);
    sUpdateDeliveryConfig->addTransition(sUpdateDeliveryConfig, &QState::entered, sIdle);

    // LoadPatient
    QObject::connect(sLoadPatient, &QState::entered, this, &MTD::LoadPatient);
    sLoadPatient->addTransition(sLoadPatient, &QState::entered, sIdle);

    // IncrementSeance
    QObject::connect(sIncrementSeance, &QState::entered, this, &MTD::UserClickIncrementSeance);
    sIncrementSeance->addTransition(sIncrementSeance, &QState::entered, sIdle);

    // AcknowledgeStarted    
    QObject::connect(sAcknowledgeStarted, &QState::entered, radiationmonitor_interface_, &medicyc::cyclotron::RadiationMonitorInterface::StartChargeMeasurement);
    QObject::connect(sAcknowledgeStarted, &QState::entered, &nj1_, &nj1::NJ1::AcknowledgeSeanceStart);
    sAcknowledgeStarted->addTransition(sAcknowledgeStarted, &QState::entered, sDelivery);

    // Delivery
    QObject::connect(sDelivery, &QState::entered, this, [&]() { QObject::connect(&nj1_, &nj1::NJ1::SIGNAL_SeancePacket, this, &MTD::SeancePacketReceived); });
    QObject::connect(sDelivery, &QState::exited, this, [&]() { QObject::disconnect(&nj1_, &nj1::NJ1::SIGNAL_SeancePacket, this, &MTD::SeancePacketReceived); });
    QObject::connect(sDelivery, &QState::entered, this, [&]() { QObject::connect(&nj1_, &nj1::NJ1::SIGNAL_SeanceFinishedPacket, this, &MTD::SeanceFinishedReceived); });
    QObject::connect(sDelivery, &QState::exited, this, [&]() { QObject::disconnect(&nj1_, &nj1::NJ1::SIGNAL_SeanceFinishedPacket, this, &MTD::SeanceFinishedReceived); });
    sDelivery->addTransition(this, &MTD::SIGNAL_SeanceFinished, sAcknowledgeFinished);
    auto tDeliveryTimeout = sDelivery->addTransition(&nj1_, &nj1::NJ1::SIGNAL_SeancePacketTimeout, sAcknowledgeFinished);
    QObject::connect(tDeliveryTimeout, &QSignalTransition::triggered, this, [&]() {
        DisplayModelessMessageBox("Connexion interrompue pendant la livraison du traitement. La dose enregistrée peut être erronée.", true, 600, QMessageBox::Critical); });

    // AcknowledgeFinished
    QObject::connect(sAcknowledgeFinished, &QState::entered, &nj1_, &nj1::NJ1::AcknowledgeSeanceFinish);
    sAcknowledgeFinished->addTransition(sAcknowledgeFinished, &QState::entered, sPostProcessResults);

    // PostProcessResults
    sPostProcessResults->setInitialState(sWait);
    QObject::connect(sPostProcessResults, &QState::entered, this, [&]() {
        ui_->genericGauge_um_seance->ConvertPrimaryToOffset();
        ui_->genericGauge_um_seance->ConvertSecondaryToOffset();
    });

    // Wait
    // No further actions

    // GetEstimatedDose
    QObject::connect(sGetEstimatedDose, &utils::TimedState::SIGNAL_Timeout, this, [&](QString message) {
        DisplayModelessMessageBox(message, true, 5, QMessageBox::Warning); });
    QObject::connect(sGetEstimatedDose, &QState::entered, this, &MTD::ClearMRMMeasurement);
    QObject::connect(sGetEstimatedDose, &QState::entered, radiationmonitor_interface_, &medicyc::cyclotron::RadiationMonitorInterface::StopChargeMeasurement);
    sGetEstimatedDose->addTransition(radiationmonitor_interface_, &medicyc::cyclotron::RadiationMonitorInterface::SIGNAL_IntegratedChargeOverInterval, sUpdateResults);
    sGetEstimatedDose->addTransition(sGetEstimatedDose, &utils::TimedState::SIGNAL_Timeout, sUpdateResults);

    // UpdateResults
    QObject::connect(sUpdateResults, &QState::entered, this, &MTD::UpdateResults);
    QObject::connect(sUpdateResults, &QState::entered, this, [&]() { if (treatment_.GetRemainingDoseDossier() < 0.1) AskIfDossierShouldBeClosed(); });
    sUpdateResults->addTransition(this, &MTD::SIGNAL_UpdateResultsDone, sIdle);

    PrintStateChanges(sSuperState, "SuperState");
    PrintStateChanges(sIdle, "Idle");
    PrintStateChanges(sUploadDeliveryConfig, "UploadDeliveryConfig");
    PrintStateChanges(sLoadPatient, "LoadPatient");
    PrintStateChanges(sUpdateDeliveryConfig, "UpdateDeliveryConfig");
    PrintStateChanges(sIncrementSeance, "IncrementSeance");
    PrintStateChanges(sAcknowledgeStarted, "AcknowledgeStarted");
    PrintStateChanges(sDelivery, "Delivery");
    PrintStateChanges(sAcknowledgeFinished, "AcknowledgeFinished");
    PrintStateChanges(sPostProcessResults, "PostProcessResults");
    PrintStateChanges(sWait, "Wait");
    PrintStateChanges(sGetEstimatedDose, "GetEstimatedDose");
    PrintStateChanges(sUpdateResults, "UpdateResults");

    sm_.addState(sSuperState);
    sm_.setInitialState(sSuperState);
    sm_.start();
}

void MTD::SeanceStartedReceived(const SeancePacket& packet) {
    try {
        n_packets_ = 0;
        if (!seance_config_.BelongsTo(packet)) {
            DisplayModelessMessageBox("La configuration de la séance envoyée depuis NJ1 ne correspond pas à la séance chargée sur MTD.", true, 600, QMessageBox::Icon::Critical);
            return;
        }
        if (!treatment_.GetActiveSeance()) {
            DisplayModelessMessageBox("Il n'y a pas de séance chargée.", true, 600, QMessageBox::Critical);
            return;
        }
//        if (treatment_.GetRemainingDoseActiveSeance() <= calc::UMToDose(static_cast<double>(seance_config_.debit()), 1)) {
//            DisplayModelessMessageBox("Il ne reste aucune dose pour cette séance.", true, 600, QMessageBox::Critical);
//            return;
//        }
        if (treatment_.IsFinished()) {
            DisplayModelessMessageBox("Ce dossier est fermé.", true, 600, QMessageBox::Critical);
            return;
        }

        seance_packets_.clear();
        seance_packets_.push_back(packet);
        UpdateUMCounters();
        AddDataPointToProgressGraph();
        emit SIGNAL_SeanceStarted();
    } catch (std::exception& exc) {
        qWarning() << "MTD::SeanceStartReceived Exception thrown: " << exc.what();
        DisplayModelessMessageBox(QString("Un erreur a été détecté: ") + QString::fromStdString(exc.what()), true, 600, QMessageBox::Critical);
    }
}

void MTD::SeancePacketReceived(const SeancePacket& packet) {
    try {
        seance_packets_.push_back(packet);
        UpdateUMCounters();
        AddDataPointToProgressGraph();
    } catch(std::exception& exc) {
        qWarning() << "MTD::SeancePacketReceived Exception thrown: " << exc.what();
    }
}

void MTD::SeanceFinishedReceived(const SeancePacket& packet) {
    try {
        seance_packets_.push_back(packet);
        UpdateUMCounters();
        AddDataPointToProgressGraph();
        seance_progress_graph_->SetCurrentAsOffset();
    } catch (std::exception& exc) {
        qWarning() << "MTD::SeanceFinishedReceived Exception thrown: " << exc.what();
    }
    emit SIGNAL_SeanceFinished();
}

void MTD::UpdateDeliveryConfig() {
    try {
        if (!treatment_.BeamCalibrationDone() || treatment_.IsFinished()) {
            DisplayModelessMessageBox("Ne peut mettre à jour les paramètres que si le débit de dose du patient est défini et qu’il reste au moins une séance", true, 600, QMessageBox::Critical);
            return;
        }
        CheckDbConnection("La connexion à la base de données est rompue.");
        SeanceParameterSelectorDialog dialog(this, seance_config_);
        if (dialog.exec() == QDialog::Accepted) {
            qDebug() << "MTD::UpdateDeliveryConfig Updating seance config";
            seance_config_ = dialog.GetSeanceConfig();
            WriteDeliveryParameters();
        } else {
            qDebug() << "MTD::UpdateDeliveryConfig Rejected changes";
        }
    }
    catch (std::exception& exc) {
        qWarning() << QString("MTD::UpdateDeliveryConfig Excpetion caught: ") + exc.what();
    }
}

void MTD::ShowConfiguration() {
    ConfigDialog configDialog;
    configDialog.show();
    configDialog.exec();
    configDialog.hide();
}

void MTD::ClearPatient() {
    ui_->label_id->setText("NA");
    ui_->label_nom->setText("NA");
    ui_->label_treatment_type->setText("NA");

    ui_->label_debit_seance->setText("NA");
    ui_->label_topdujour->setText("NA");
    ui_->label_debitpatient->setText("NA");

    ui_->label_dose->setText("NA");
    ui_->label_seance_number->setText("NA");
    ui_->label_um->setText("NA");
    ui_->label_current_stripper->setText("NA");
    ui_->label_duration->setText("NA");

    ui_->label_rangeshifter->setText("NA");
    ui_->label_modulator->setText("NA");

    treatment_ = Treatment();
}

void MTD::UploadDeliveryConfig() {
    if (!treatment_.BeamCalibrationDone()) {
        emit SIGNAL_Abort();
        DisplayModelessMessageBox("Ne peut envoyer les donnéés que si le débit de dose du patient est défini", true, 5, QMessageBox::Warning);
        return;
    }
    if (treatment_.IsFinished()) {
        emit SIGNAL_Abort();
        DisplayModelessMessageBox("Ne peut envoyer les donnéés que s’il reste au moins une séance", true, 5, QMessageBox::Warning);
        return;
    }
    if (seance_config_.mu_des() == 0) {
        emit SIGNAL_Abort();
        DisplayModelessMessageBox("0 UM non autorisé comme configuration", true, 5, QMessageBox::Warning);
        return;
    }
    if (!nj1_.IsConnected()) {
        emit SIGNAL_Abort();
        DisplayModelessMessageBox("Pas de connexion a l'automate", true, 5, QMessageBox::Warning);
        return;
    }

    seance_progress_graph_->Clear();
    seance_progress_graph_->SetDoseRange(0.0, static_cast<double>(seance_config_.mu_des())*1.1);
    seance_progress_graph_->SetTimeRange(0.0, static_cast<double>(seance_config_.duration()/1000)*1.2);
    nj1_.UploadNewSeanceConfig(seance_config_);
    ConfigureUMSeanceGauge(); // needed if a 2nd intrafraction is uploaded from MTD instead of continued locally on the NJ1
}

// can throw
void MTD::UpdateUMCounters() {
    ui_->genericGauge_um_seance->SetPrimaryValue(static_cast<double>(GetUM1()));
    ui_->genericGauge_um_seance->SetSecondaryValue(static_cast<double>(GetUM2()));
}

// Can throw
uint32_t MTD::GetUM1() {
    if (seance_packets_.empty()) {
        return 0;
    }
    if (seance_packets_.back().um1() < seance_packets_.front().um1()) {
        throw std::runtime_error("UM competur 1 corrumpu");
    }
    return (seance_packets_.back().um1() - seance_packets_.front().um1());
}

// Can throw
uint32_t MTD::GetUM2() {
    if (seance_packets_.empty()) {
        return 0;
    }
    if (seance_packets_.back().um2() < seance_packets_.front().um2()) {
        throw std::runtime_error("UM competur 2 corrumpu");
    }
    return (seance_packets_.back().um2() - seance_packets_.front().um2());
}

void MTD::UpdateResults() {
    try {
        CheckDbConnection("La connexion à la base de données est rompue. La dose ne sera pas enregistrée.");

        // Convert and calculate the results
        SeanceRecord seance_record = seance_config_.Convert(QDateTime::currentDateTime(), seance_packets_);
        seance_record.SetEstimatedDose(mrm_estimated_dose_);

        // add the results to data structures and to the db
        treatment_.GetActiveSeance()->AddSeanceRecord(seance_record);
        repo_->SaveSeanceRecord(treatment_.GetActiveSeance()->GetSeanceId() , seance_record);
        SaveMRMInfo(seance_record);

        // update graphics with data from delivered beam
        ui_->genericGauge_dose_total->SetPrimaryValue(treatment_.GetDeliveredDose());
        ui_->genericGauge_dose_total->SetSecondaryValue(treatment_.GetEstimatedDeliveredDose());
        ui_->genericGauge_dose_seance->SetPrimaryValue(treatment_.GetActiveSeance()->GetDoseDelivered());
        ui_->genericGauge_dose_seance->SetSecondaryValue(treatment_.GetActiveSeance()->GetDoseEstimated());
        seance_dose_graph_->SetData(treatment_.GetAllSeances());

        // Prepare next intrafraction
        seance_config_ = treatment_.GetSeanceConfig();

        WriteDeliveryParameters();
        emit SIGNAL_UpdateResultsDone();
    }
    catch (std::exception& exc) {
        emit SIGNAL_UpdateResultsDone(); // Put it before blocking qmessagebox
        qWarning() << QString("MTD::UpdateResults Exception caught : ") + exc.what();
        QString message = QString("Il y avait un problém avec les donnéés recus: ") + exc.what() + " La dose délivrée n'a peut-être pas été correctement enregistrée.";
        DisplayModelessMessageBox(message, true, 600, QMessageBox::Warning);
    }

}

void MTD::AskIfDossierShouldBeClosed() {
    const double remaining_dose = treatment_.GetRemainingDoseDossier();
    QMessageBox *box = new QMessageBox(this);
    QString message;
    if (remaining_dose < 0.0) {
        message = "Toute la dose prevue a été livréé pour cette patient\n(Dépassement: " + QString::number(std::abs(remaining_dose), 'f', 3) + " Gy)";
    } else {
        message = "Toute la dose prevue a été livrée pour cette patient\n(Reste: " + QString::number(remaining_dose, 'f', 3) + " Gy)";
    }
    box->setText(message);
    box->setInformativeText("Le dossier est terminé?");
    box->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    box->setDefaultButton(QMessageBox::Yes);
    box->setModal(true); // set true
    QObject::connect(box, &QMessageBox::accepted, this, &MTD::CloseDossier);
    box->show();
 }

void MTD::CloseDossier() {
    try {
        if (!DossierLoaded()) {
            DisplayModelessMessageBox("Un dossier n'est pas chargé", true, 10, QMessageBox::Warning);
            return;
        }

        if (treatment_.IsFinished()) {
            DisplayModelessMessageBox("Le dossier est déjà fermé", true, 10, QMessageBox::Warning);
            return;
        }

        CheckDbConnection("La connexion à la base de données est rompue. Le dossier ne sera pas fermé.");
        treatment_.Close();
        seance_config_ = treatment_.GetSeanceConfig();
        repo_->UpdateTreatment(treatment_);
        WriteDeliveryParameters();
    }
    catch(std::exception& exc) {
        DisplayModelessMessageBox(exc.what(), true, 10, QMessageBox::Critical);
    }
}

void MTD::DoIncrementSeance() {
    try {
        CheckDbConnection("La connexion à la base de données est rompue. La séance ne sera pas changé.");
        treatment_.IncrementSeance();
        repo_->UpdateTreatment(treatment_);
        seance_config_ = treatment_.GetSeanceConfig();
        ConfigureUMSeanceGauge();
        ConfigureDoseSeanceGauge();
        WriteDeliveryParameters();
    }
    catch (std::exception& exc) {
        qWarning() << "MTD::IncrementSeance Caught exception: " << exc.what();
        DisplayModelessMessageBox("Exception thrown: " + QString(exc.what()), true, 10, QMessageBox::Warning);
    }
}

bool MTD::UserConfirmationIncrementSeance() {
    const int active_seance(treatment_.GetActiveSeanceNumber() + 1); // starts from 0
    QString msg = QString("Passer de la séance ") + QString::number(active_seance) +
            " à la séance " + QString::number(active_seance + 1) + "?" + "\n\n";
    if (treatment_.BeamCalibrationDone()) {
        const uint32_t remaining_um = static_cast<uint32_t>(treatment_.GetUMPrevuActiveSeance());
        msg.append(QString::number(remaining_um) + " UM sera ajoutes à la prochaine séance");
    } else {
        msg.append(QString::number(treatment_.GetRemainingDoseActiveSeance(), 'f', 2) + QString(" Gy sera ajoutes à la prochaine séance"));
    }
    QMessageBox msg_box;
    msg_box.setText(msg);
    msg_box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msg_box.setDefaultButton(QMessageBox::No);
    return (msg_box.exec() == QMessageBox::Yes);
}

bool MTD::AdminConfirmationIncrementSeance() {
    QString msg;
    msg.append("Vous n'êtes pas autorisé à transférer plus de 10% de la dose d'une séance à l'autre.");
    QMessageBox msg_box;
    msg_box.setIcon(QMessageBox::Critical);
    msg_box.setText(msg);
    QPushButton *reject_button = msg_box.addButton("Passer outre", QMessageBox::RejectRole);
    QPushButton *accept_button = msg_box.addButton("Ok", QMessageBox::AcceptRole);
    msg_box.setDefaultButton(accept_button);
    msg_box.exec();
    if (msg_box.clickedButton() == reject_button) {
        return true;
    } else {
        qDebug() << "MTD::AdminConfirmationIncrementSeance To high dose to transfer between seances, aborting.";
        return false;
    }
}

void MTD::UserClickIncrementSeance() {
    if (!DossierLoaded()) {
        DisplayModelessMessageBox("Un dossier n'est pas chargé", true, 10, QMessageBox::Warning);
        return;
    }

    if (treatment_.IsFinished()) {
        DisplayModelessMessageBox("Le dossier est déjà fermé", true, 10, QMessageBox::Warning);
        return;
    }

    if (!treatment_.GetActiveSeance()) {
        DisplayModelessMessageBox("Aucune séance n'est actuellement active", true, 10, QMessageBox::Warning);
        return;
    }

    // TODO: abort if no seance records in active seance

    if (treatment_.IsLastSeance()) {
        QMessageBox msg_box;
        msg_box.setText(QString("Cela fermera le dossier. Continuer?"));
        msg_box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msg_box.setDefaultButton(QMessageBox::No);
        if (msg_box.exec() == QMessageBox::Yes) {
            DoIncrementSeance();
        }
        return;
    }

    if (!UserConfirmationIncrementSeance()) {
        return;
    }

    const double desired_transfer_dose = treatment_.GetRemainingDoseActiveSeance();
    const double allowed_transfer_dose = 0.1*treatment_.GetActiveSeance()->GetDosePrescribed();
    if (desired_transfer_dose < allowed_transfer_dose) {
        DoIncrementSeance();
    } else if (AdminConfirmationIncrementSeance()) {
        int ret = 0;
        while(ret == 0) { // 0 = password wrong, 1 = password correct, 2 = cancel
            PasswordConfirmation password(this, "Admin", "JH06");
            password.setVisible(true); password.show();
            ret = password.exec();
            if (ret == 1) {
                DoIncrementSeance();
                qDebug() << "MTD::IncrementSeance Password Ok";
            }
        }
    }
}

void MTD::AddDataPointToProgressGraph() {
    seance_progress_graph_->AddData(static_cast<double>(seance_packets_.back().duration())/1000.0,
                                    GetUM1(),
                                    GetUM2(),
                                    static_cast<double>(seance_packets_.back().i_ct1()),
                                    static_cast<double>(seance_packets_ .back().i_ct2()),
                                    static_cast<double>(seance_packets_.back().cf9_status()));
}

void MTD::LoadPatient() {
    try {
        CheckDbConnection("La connexion à la base de données est rompue.");
        DossierSelectionDialog dialog(this, repo_);
        const int response = dialog.exec();
        if (response == QDialog::Accepted) {
            ClearPatient();
            qDebug() << "MTD::LoadDossier Accepted " << dialog.dossier();
            treatment_ = repo_->GetTreatment(dialog.dossier());
            Defaults def = repo_->GetDefaults();
            seance_config_ = treatment_.GetSeanceConfig();

            FillPatientInfo();
            FillModulatingInfo();
            ConfigureUMSeanceGauge();
            ConfigureDoseSeanceGauge();
            ConfigureDoseTotalGauge();
            seance_dose_graph_->SetData(treatment_.GetAllSeances());
            seance_progress_graph_->Clear();
            WriteBeamCalibration();
            WriteDeliveryParameters();
        } else if (response == 2) {
            qDebug() << "MTD::LoadDossier canceled";
        } else {
            qDebug() << "MTD::LoadDossier Rejected";
            DisplayModelessMessageBox("Echec du chargement du patient", true, 10, QMessageBox::Warning);
        }
    }
    catch (std::exception &exc) {
        qWarning() << QString("MTD::LoadPatient Excpetion caught: ") + exc.what();
        DisplayModelessMessageBox("Echec du chargement du patient", true, 10, QMessageBox::Warning);
    }
}

void MTD::FillPatientInfo() {
    ui_->label_id->setText(QString::number(treatment_.GetDossier()));
    QString name = treatment_.GetPatient().GetLastName().toUpper() + " " + treatment_.GetPatient().GetFirstName();
    if (name.size() > 20) {
        name = name.left(20) + "...";
    }
    qDebug() << "Name size " << name.size() << " " << name;
    ui_->label_nom->setText(name);
    ui_->label_treatment_type->setText(treatment_.TreatmentType());
}

void MTD::WriteBeamCalibration() {
    try {
        ui_->label_debit_seance->setText("NA");
        ui_->label_debitpatient->setText("NA");
        ui_->label_topdujour->setText("NA");

        Dosimetry patientDosi = treatment_.GetPatientDosimetry();
        Dosimetry patientTopDuJour = treatment_.GetPatientRefDosimetry();
        Dosimetry topDuJour = treatment_.GetRefDosimetryToday();

        if (topDuJour.IsValid()) {
            ui_->label_topdujour->setText(QString::number(topDuJour.GetDebitMean(), 'f', 4));
        }
        if (patientDosi.IsValid()) {
            if (patientDosi.IsMonteCarlo()) {
                ui_->label_debitpatient->setText(QString::number(patientDosi.GetDebitMean(), 'f', 4));
            } else if (patientTopDuJour.IsValid()){
                ui_->label_debitpatient->setText(QString::number(patientDosi.GetDebitMean() * treatment_.dref() / patientTopDuJour.GetDebitMean(), 'f', 4));
            }
        }

        if (treatment_.BeamCalibrationDone()) {
            const QString debit(QString::number(treatment_.GetBeamCalibration(), 'f', 4));
            const QString debitStdDev(QString::number(treatment_.GetBeamCalibrationStdDev(), 'f', 4));
            ui_->label_debit_seance->setText(debit + " \u00B1 " + debitStdDev);
        }
    }
    catch (std::exception& exc) {
        qWarning() << QString("MTD::WriteBeamCalibration There was en exception when computing the beam calibration: ") + exc.what();
        DisplayModelessMessageBox("Une erreur s'est produite lors du calcul de l'étalonnage du faisceau.", true, 10, QMessageBox::Warning);
    }
}

void MTD::WriteDeliveryParameters() {
    try {
        // write dose in all cases, in case there remains dose but all seances are finished (by operator mistake)
        ui_->label_dose->setText(QString::number(static_cast<double>(seance_config_.dose()), 'f', 2));

        ui_->label_seance_number->setText("NA");
        ui_->label_um->setText("NA");
        ui_->label_current_stripper->setText("NA");
        ui_->label_duration->setText("NA");

        if (!treatment_.IsFinished()) {
            ui_->label_seance_number->setText(QString::number(seance_config_.seance_id()));
            if (treatment_.BeamCalibrationDone()) {
                ui_->label_duration->setText(QString::number(static_cast<double>(seance_config_.duration()) / 1000.0, 'f', 1));
                ui_->label_um->setText(QString::number(seance_config_.mu_des()));
                ui_->label_current_stripper->setText(QString::number(seance_config_.current_stripper()));
            }
        }
    }
    catch (std::exception& exc) {
        qWarning() << QString("MTD::WriteDeliveryParameters There was en exception: ") + exc.what();
        DisplayModelessMessageBox("Une erreur s'est produite lors du préparation du séance", true, 10, QMessageBox::Warning);
    }
}

void MTD::FillModulatingInfo() {
    ui_->label_rangeshifter->setText(treatment_.GetDegradeurSet().mm_plexis().join(","));
    ui_->label_modulator->setText(treatment_.GetModulateur().id());
}

void MTD::ConfigureUMSeanceGauge() {
    ui_->genericGauge_um_seance->Reset();
    ui_->genericGauge_um_seance->SetLimits(0.0f, static_cast<double>(seance_config_.mu_des()));
    ui_->genericGauge_um_seance->SetPrimaryValue(0.0f);
    ui_->genericGauge_um_seance->SetSecondaryValue(0.0f);
}

void MTD::ConfigureDoseSeanceGauge() {
    if (treatment_.GetActiveSeance()) {
        ui_->genericGauge_dose_seance->Reset();
        ui_->genericGauge_dose_seance->SetLimits(0.0f, treatment_.GetDesiredDoseActiveSeance());
        ui_->genericGauge_dose_seance->SetPrimaryValue(treatment_.GetActiveSeance()->GetDoseDelivered());
        ui_->genericGauge_dose_seance->SetSecondaryValue(treatment_.GetActiveSeance()->GetDoseEstimated());
    }
}

void MTD::ConfigureDoseTotalGauge() {
    ui_->genericGauge_dose_total->Reset();
    ui_->genericGauge_dose_total->SetLimits(0.0f, treatment_.GetPrescribedDose());
    ui_->genericGauge_dose_total->SetPrimaryValue(treatment_.GetDeliveredDose());
    ui_->genericGauge_dose_total->SetSecondaryValue(treatment_.GetEstimatedDeliveredDose());
}

void MTD::SetupDb() {
    if (repo_) {
        delete repo_;
        repo_ = nullptr;
    }
    TurnOffDbButton();
    QString dbHostName = settings_->value("database/host", "unknown").toString();
    int dbPort = settings_->value("database/port", "5432").toInt();
    QString dbDatabaseName = settings_->value("database/name", "unknown").toString();
    QString dbUserName = settings_->value("database/user", "unknown").toString();
    QString dbPassword = settings_->value("database/password", "unknown").toString();
    QString dbConnName = "patientDb";
    int deconnect_on_idle_timeout = settings_->value("database/disconnect_on_idle_timeout", "600").toInt();

    if (dbHostName == "unknown") qCritical() << "MTD::MTD Unknown db hostname";
    if (dbDatabaseName == "unknown") qCritical() << "MTD::MTD Unknown db name";
    if (dbUserName == "unknown") qCritical() << "MTD::MTD Unknown db username";
    if (dbPassword == "unknown") qCritical() << "MTD::MTD Unknown db password";

    repo_ = new PTRepo(dbConnName, dbHostName, dbPort, dbDatabaseName, dbUserName, dbPassword, deconnect_on_idle_timeout);
    QObject::connect(repo_, &PTRepo::SIGNAL_ConnectionClosed, this, &MTD::TurnOffDbButton);
    QObject::connect(repo_, &PTRepo::SIGNAL_ConnectionOpened, this, &MTD::TurnOnDbButton);
    QObject::connect(repo_, &PTRepo::SIGNAL_FailedOpeningConnection, this, &MTD::FailedConnectingToDatabase);
    QObject::connect(repo_, &PTRepo::SIGNAL_FailedOpeningConnection, this, &MTD::TurnOffDbButton);
    repo_->Connect();
}

void MTD::FailedConnectingToDatabase() {
    DisplayModelessMessageBox("Echec de la connexion à la base de données", true, 60, QMessageBox::Critical);
}

bool MTD::DossierLoaded() const {
    return (treatment_.GetDossier() != 0);
}

void MTD::ShowCollimator() {
    try {
        if (!DossierLoaded()) {
            DisplayModelessMessageBox("Un dossier n'est pas chargé", true, 60, QMessageBox::Warning);
            return;
        }
        CheckDbConnection("La connexion à la base de données est rompue.");
        auto collimator = repo_->GetCollimator(treatment_.GetDossier());
        CollimateurDialog collimateurDialog(collimator);
        collimateurDialog.exec();
    }
    catch (std::exception& exc) {
        qWarning() << "MTD::ShowCollimateur Catched an exception: " << exc.what();
        DisplayModelessMessageBox("Un collimateur n'a pas été trouvé", true, 60, QMessageBox::Warning);
    }
}

void MTD::ShowCompensator() {
    try {
        if (!DossierLoaded()) {
            DisplayModelessMessageBox("Un dossier n'est pas chargé", true, 60, QMessageBox::Warning);
            return;
        }
        CheckDbConnection("La connexion à la base de données est rompue.");
        auto compensateur = repo_->GetCompensateur(treatment_.GetDossier());
        CompensateurTransparantDialog dialog(this, compensateur, treatment_.GetPatient());
        dialog.exec();
    }
    catch (std::exception& exc) {
        qWarning() << "MTD::ShowCompensateur Catched an exception: " << exc.what();
        DisplayModelessMessageBox("Un compensateur n'a pas été trouvé", true, 60, QMessageBox::Warning);
    }
}

void MTD::ShowSeanceInfo(int seance_idx) {
    try {
       auto seance = treatment_.GetSeance(seance_idx);
       QString text("");
       text.append("Dose planifiée:   " + QString::number(seance.GetDosePrescribed(), 'f', 3) + " Gy\n");
       text.append("Dose delivree: " + QString::number(seance.GetDoseDelivered(), 'f', 3) + " Gy\n");
       auto records = seance.GetSeanceRecords();
       if (records.empty()) {
           text.append("Aucun faisceau envoyé\n");
       } else {
           int rec_nmb(1);
           for (auto& rec : records) {
               text.append("\nFaisceau:         " + QString::number(rec_nmb++) + "\n");
               text.append("Horodatage:         " + rec.GetTimestamp().toString() + "\n");
               text.append("Durée:              " + QString::number(rec.GetTotalDuration(), 'f', 1) + " s\n");
               text.append("I.Stripper:         " + QString::number(rec.GetIStripper(), 'f', 0) + " nA\n");
               text.append("I.CF9:              " + QString::number(rec.GetICF9(), 'f', 0) + " nA\n");
               text.append("I.Chambre1:         " + QString::number(calc::Mean(rec.GetIChambre1()), 'f', 0) + " nA\n");
               text.append("I.Chambre2:         " + QString::number(calc::Mean(rec.GetIChambre2()), 'f', 0) + " nA\n");
               text.append("Débit:              " + QString::number(rec.GetDebit(), 'f', 4) + " cGy/UM\n");
               text.append("UM Prevu:           " + QString::number(rec.GetUMPrevu()) + "\n");
               text.append("UM Délivrée 1:      " + QString::number(rec.GetTotalUM1Delivered()) + "\n");
               text.append("UM Délivrée 2:      " + QString::number(rec.GetTotalUM2Delivered()) + "\n");
               text.append("Dose délivrée (Gy): " + QString::number(rec.GetDoseDelivered(), 'f', 3) + "\n");
               text.append("Dose estimée (Gy):  " + QString::number(rec.GetDoseEstimated(), 'f', 3) + "\n");
           }
       }
       DisplayModelessMessageBox(QString("Seance ") + QString::number(seance_idx + 1) + "\n\n" + text, false, 20, QMessageBox::Information);
    }
    catch (std::exception const& exc) {
        qWarning() << QString("TreatmentTab::ShowSeanceInfo Exception thrown: ") + exc.what();
        DisplayModelessMessageBox(QString("Seance") + QString::number(seance_idx + 1) + "\n\nData not found", true, 10, QMessageBox::Warning);
    }
}

void MTD::TurnOffNJ1Button() {
    ui_->pushButton_nj1->setStyleSheet(ButtonStyleSheet("button_red_40.png", 20));
}

void MTD::TurnOnNJ1Button() {
    ui_->pushButton_nj1->setStyleSheet(ButtonStyleSheet("button_green_40.png", 20));
}

void MTD::TurnOffDbButton() {
    ui_->pushButton_db->setStyleSheet(ButtonStyleSheet("db_button_red.png", 20));
}

void MTD::TurnOnDbButton() {
    ui_->pushButton_db->setStyleSheet(ButtonStyleSheet("db_button_green.png", 20));
}

QString MTD::ButtonStyleSheet(QString image, int radius) {
    QString str ("QPushButton {"
                 "background-image: url(:/images/" + image + ");"
    "background-repeat: no-repeat;"
    "background-position: center;"
    "border-style: raised; "
    "border-width: 0px;"
    "border-radius: " + QString::number(radius) + "px;"
    "color: white;"
    "};"
    "QPushButton:pressed {"
    "border-width: 4px."
    "};");
    return str;
}

void MTD::StateMachineMessage(QString message) {
    qDebug() << "MTD::StateMachine " << QDateTime::currentDateTime() << " " << message;
    ui_->label_sm_state->setText(message);
}

void MTD::ClearMRMMeasurement() {
    mrm_measurement_ok_ = false;
    mrm_estimated_dose_ = 0.0;
    mrm_integrated_charge_ = 0.0;
}

void MTD::CalculateEstimatedDose(double ambient_charge) {
    mrm_integrated_charge_ = ambient_charge;
    mrm_estimated_dose_ = 0.0;
    mrm_measurement_ok_ = false;
    try {
        CheckDbConnection("La connexion à la base de données est rompue. La dose MRM ne sera pas calculéé");
        static BaliseCalibration calibration = repo_->GetBaliseCalibration();
        mrm_estimated_dose_ = algorithms::CalculateIntegratedDose(calibration.DoseRateToH_alpha(),
                                                                  calibration.DoseRateToH_beta(),
                                                                  calibration.svPerCoulomb(),
                                                                  ambient_charge,
                                                                  treatment_.GetBeamCalibration());
        qDebug() << "MTD::CalculateEstimatedDose charge " << mrm_integrated_charge_ << " dose " << mrm_estimated_dose_;
        mrm_measurement_ok_ = true;
    }
    catch (std::exception& exc) {
        qWarning() << "MTD::CalculateEstimatedDose " << QDateTime::currentDateTime() << " Exception caught " << exc.what();
        DisplayModelessMessageBox("La dose calculée à partir du rayonnement secondaire n'a pas pu être calculée", true, 10, QMessageBox::Warning);
    }
}

void MTD::SaveMRMInfo(const SeanceRecord& seance_record) {
    try {
        if (mrm_measurement_ok_) {
            CheckDbConnection("La connexion à la base de données est rompue. Les donnéés MRM ne seront pas enregistrées.");
            Dosimetry patient_dosi = treatment_.GetPatientDosimetry();
            const double patient_debit_ref_conditions = (patient_dosi.IsMonteCarlo() ? patient_dosi.GetDebitMean() :
                                                                                       patient_dosi.GetDebitMean() * treatment_.dref() / treatment_.GetPatientRefDosimetry().GetDebitMean());
            BaliseRecord balise_record(QDateTime::currentDateTime(),
                                       treatment_.GetDossier(),
                                       repo_->GetSeanceRecordId(seance_record),
                                       mrm_integrated_charge_,
                                       treatment_.dref(),
                                       treatment_.GetRefDosimetryToday().GetDebitMean(),
                                       patient_debit_ref_conditions,
                                       seance_record.GetDoseDelivered());
            repo_->SaveBaliseRecord(balise_record);
        }
    }
    catch (std::exception& exc) {
        qWarning() << "MTD::SaveMRMRecord " << QDateTime::currentDateTime() << " Exception caught " << exc.what();
        DisplayModelessMessageBox("Les informations associées à MRM n'ont pas pu être sauvegardées dans la base de données", true, 60, QMessageBox::Warning);
    }
}

void MTD::CheckDbConnection(QString message) {
    if (!repo_->CheckConnection()) {
        throw std::runtime_error(message.toStdString());
    }
}

void MTD::DisplayModelessMessageBox(QString msg, bool auto_close, int sec, QMessageBox::Icon icon) {
    qDebug() << "MTD::DisplayModelessMessageBox " << msg;
    QMessageBox* box = new QMessageBox(this);
    box->setText(msg);
    box->setIcon(icon);
    box->setModal(false);
    box->setAttribute(Qt::WA_DeleteOnClose);
    if (auto_close) QTimer::singleShot(sec * 1000, box, SLOT(close()));
    box->show();
}

void MTD::PrintStateChanges(QState *state, QString name) {
    QObject::connect(state, &QState::entered, this, [&, name]() { qDebug() << "MTD " << QDateTime::currentDateTime() << " ->" << name; });
    QObject::connect(state, &QState::exited, this, [&, name]() { qDebug() << "MTD " << QDateTime::currentDateTime() << " <-" << name; });
}
