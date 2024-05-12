#include "MRM.h"

#include <QFinalState>
#include <QEventTransition>
#include <QSignalTransition>
#include <QtDBus/QDBusConnection>
#include <memory>
#include <QTimer>
#include <unistd.h>

#include "Algorithms.h"
#include "ClientConnection.h"
#include "XRayRecord.h"
#include "TimedState.h"

#include "MRMDBusAdaptor.h"

MRM::MRM(PTRepo *repo,
               BaliseConfiguration balise_configuration,
               ClientConnection *connection,
               QSettings* settings)
    : repo_(repo),
      configuration_(balise_configuration),
      client_connection_(connection),
      ring_buffer_(300),
      xray_identifier_(settings->value("xray/min_in_interval", "1").toInt(), settings->value("xray/max_in_interval", "3").toInt(),
                       settings->value("xray/lower_interval", "0.8E-5").toDouble(), settings->value("xray/upper_interval", "8.0E-5").toDouble()),
      background_level_(settings->value("misc/background_level", "4.0E-6").toDouble()),
      continuous_save_(settings->value("misc/continuous_save", "false").toBool()),
      reconnect_interval_(settings->value("comm/reconnect_interval", "3000").toInt()),
      settings_(settings)
{
    SetupDBus();
    SetupCommandStateMachine();
    SetupConfiguringStateMachine();
    SetupMeasurementStateMachine();
    SetupReadBufferStateMachine();
    SetupMeasurementCheckTimer();

    ConnectClientConnectionSignals();;
    ConnectReplyParserSignals();
    ConnectHardware();
    ConfigureTick();
}

MRM::~MRM() {
    delete client_connection_;
}

void MRM::SetupDBus() {
    QString dbus_host = settings_->value("dbus/host", "").toString();
    QString dbus_port = settings_->value("dbus/port", "").toString();
    QString dbus_name = settings_->value("dbus/name", "").toString();
    QString dbus_conn = QString("tcp:host=%1,port=%2").arg(dbus_host).arg(dbus_port);
    qputenv("DBUS_SESSION_BUS_ADDRESS", dbus_conn.toUtf8());
    qDebug() << qgetenv("DBUS_SESSION_BUS_ADDRESS");
    new MRMDBusAdaptor(this);
    QDBusConnection connection = QDBusConnection::connectToBus(dbus_conn, dbus_name);
    connection.registerObject("/MRM", this);
    connection.registerService("MedicycPT.MRM");
    qDebug() << "MRM::SetupDBus Status " << connection.name() << " " << connection.isConnected();
}

void MRM::ConnectHardware() {
    client_connection_->Connect();
}

void MRM::ConnectClientConnectionSignals() {
    QObject::connect(dynamic_cast<QObject*>(client_connection_), SIGNAL(DataRead(QByteArray)), &reply_parser_, SLOT(Parse(QByteArray)));
    QObject::connect(dynamic_cast<QObject*>(client_connection_), SIGNAL(ReadError(QString)), this, SIGNAL(CommandError(QString)));
    QObject::connect(dynamic_cast<QObject*>(client_connection_), SIGNAL(WriteError(QString)), this, SIGNAL(CommandError(QString)));
    QObject::connect(dynamic_cast<QObject*>(client_connection_), SIGNAL(PortError(QString)), this, SIGNAL(CommandError(QString)));
    QObject::connect(dynamic_cast<QObject*>(client_connection_), SIGNAL(ConnectionEstablished()), this, SIGNAL(Connected()));
    QObject::connect(dynamic_cast<QObject*>(client_connection_), SIGNAL(PortError(QString)), this, SIGNAL(Disconnected()));

    QObject::connect(this, &MRM::CommandError, this, [this](QString msg) { this->error_message_ = msg; });
}

void MRM::ConnectReplyParserSignals() {
    QObject::connect(&reply_parser_, &ReplyParser::DateChanged, this, [=](QDateTime date) { current_timestamp_ = date; });
    QObject::connect(&reply_parser_, &ReplyParser::ReplyError, this, &MRM::ReplyError);
    QObject::connect(&reply_parser_, &ReplyParser::BaliseError, this, &MRM::MRMError);
    QObject::connect(&reply_parser_, &ReplyParser::InstantenousMeasurementChanged, this, &MRM::InstantenousMeasurementChanged);

    // XRAY
    QObject::connect(this, &MRM::InstantenousMeasurementChanged, &xray_identifier_, &XRayIdentifier::AddInstantaneousValue);
    QObject::connect(&xray_identifier_, &XRayIdentifier::XRayIdentified, this, &MRM::HandleDetectedXRay);

    QObject::connect(this, &MRM::InstantenousMeasurementChanged, this,
                     [=](InstantenousMeasurement m) { if (BackgroundLevel(m.conversed_value())) emit(BackgroundLevel()); });

    // SAVE RADIATION LEVELS TO DB
    if (continuous_save_) {
        QObject::connect(this, &MRM::InstantenousMeasurementChanged, this,
                         [=](InstantenousMeasurement m) { instantaneous_values_.push_back(BaliseLevel(m.timestamp(), m.conversed_value())); });
        QObject::connect(this, &MRM::IntegratedMeasurement1Changed, this,
                         [=](IntegratedMeasurement m) { integrated_values_.push_back(BaliseLevel(m.timestamp(), m.charge())); });
        QObject::connect(this, &MRM::BufferIntegratedMeasurement1Changed, this,
                         [=](IntegratedMeasurementBuffer m) { buffer_values_.push_back(BaliseLevel(m.ActualMeasurement().timestamp(), m.ActualMeasurement().charge())); });
    }

    QObject::connect(&reply_parser_, &ReplyParser::CommandConfirmed, this, [=]() { configuration_.CommandConfirmationChanged(true); });
    QObject::connect(&reply_parser_, &ReplyParser::IntegratedMeasurement1Changed, this, &MRM::AddIntegratedToRingBuffer);
    QObject::connect(&reply_parser_, &ReplyParser::IntegratedMeasurement1Changed, this, &MRM::IntegratedMeasurement1Changed);
    QObject::connect(&reply_parser_, &ReplyParser::IntegratedMeasurement2Changed, this, &MRM::IntegratedMeasurement2Changed);
    QObject::connect(&reply_parser_, &ReplyParser::BufferIntegratedMeasurement1Changed, this, &MRM::AddBufferedToRingBuffer);
    QObject::connect(&reply_parser_, &ReplyParser::BufferIntegratedMeasurement1Changed, this, &MRM::BufferIntegratedMeasurement1Changed);
    QObject::connect(&reply_parser_, &ReplyParser::BufferIntegratedMeasurement2Changed, this, &MRM::BufferIntegratedMeasurement2Changed);
    QObject::connect(&reply_parser_, &ReplyParser::ThresholdStatusChanged, this, &MRM::ThresholdStatusChanged);
    QObject::connect(&reply_parser_, &ReplyParser::ModeFunctionnementChanged, this, &MRM::ModeFunctionnementChanged);
    QObject::connect(&reply_parser_, &ReplyParser::ModeFunctionnementChanged, &configuration_, &Configuration::ModeFunctionnementChanged);
    QObject::connect(&reply_parser_, &ReplyParser::PreAmpliTypeChanged, this, &MRM::PreAmpliTypeChanged);
    QObject::connect(&reply_parser_, &ReplyParser::PreAmpliTypeChanged, &configuration_, &Configuration::PreAmpliTypeChanged);
    QObject::connect(&reply_parser_, &ReplyParser::PreAmpliGammeChanged, this, &MRM::PreAmpliGammeChanged);
    QObject::connect(&reply_parser_, &ReplyParser::PreAmpliGammeChanged, &configuration_, &Configuration::PreAmpliGammeChanged);
    QObject::connect(&reply_parser_, &ReplyParser::EmitInstantenousMeasurementChanged, this, &MRM::EmitInstantenousMeasurementChanged);
    QObject::connect(&reply_parser_, &ReplyParser::EmitInstantenousMeasurementChanged, &configuration_, &Configuration::InstantaneousLAMChanged);
    QObject::connect(&reply_parser_, &ReplyParser::EmitIntegrated1MeasurementChanged, this, &MRM::EmitIntegrated1MeasurementChanged);
    QObject::connect(&reply_parser_, &ReplyParser::EmitIntegrated1MeasurementChanged, &configuration_, &Configuration::Integrated1LAMChanged);
    QObject::connect(&reply_parser_, &ReplyParser::EmitIntegrated2MeasurementChanged, this, &MRM::EmitIntegrated2MeasurementChanged);
    QObject::connect(&reply_parser_, &ReplyParser::EmitIntegrated2MeasurementChanged, &configuration_, &Configuration::Integrated2LAMChanged);
    QObject::connect(&reply_parser_, &ReplyParser::IncludeDateInMeasurementChanged, this, &MRM::IncludeDateInMeasurementChanged);
    QObject::connect(&reply_parser_, &ReplyParser::IncludeDateInMeasurementChanged, &configuration_, &Configuration::IncludeDateInMeasurementChanged);
    QObject::connect(&reply_parser_, &ReplyParser::IncludeConversedValueInMeasurementChanged, this, &MRM::IncludeConversedValueInMeasurementChanged);
    QObject::connect(&reply_parser_, &ReplyParser::IncludeConversedValueInMeasurementChanged, &configuration_, &Configuration::IncludeRawInMeasurementChanged);
    QObject::connect(&reply_parser_, &ReplyParser::DateChanged, this, &MRM::DateChanged);
    QObject::connect(&reply_parser_, &ReplyParser::InstantenousMeasurementTimeChanged, this, &MRM::InstantenousMeasurementTimeChanged);
    QObject::connect(&reply_parser_, &ReplyParser::InstantenousMeasurementTimeChanged, &configuration_, &Configuration::InstantaneousTimeChanged);
    QObject::connect(&reply_parser_, &ReplyParser::InstantenousMeasurementElementsChanged, this, &MRM::InstantenousMeasurementElementsChanged);
    QObject::connect(&reply_parser_, &ReplyParser::InstantenousMeasurementElementsChanged, &configuration_, &Configuration::InstantaneousElementsChanged);
    QObject::connect(&reply_parser_, &ReplyParser::NmbMeasurementsBelowThresholdBeforeAlarmResetChanged, this, &MRM::NmbMeasurementsBelowThresholdBeforeAlarmResetChanged);
    QObject::connect(&reply_parser_, &ReplyParser::IntegratedMeasurement1TimeChanged, this, &MRM::IntegratedMeasurement1TimeChanged);
    QObject::connect(&reply_parser_, &ReplyParser::IntegratedMeasurement1TimeChanged, &configuration_, &Configuration::Integrated1TimeChanged);
    QObject::connect(&reply_parser_, &ReplyParser::IntegratedMeasurement2TimeChanged, this, &MRM::IntegratedMeasurement2TimeChanged);
    QObject::connect(&reply_parser_, &ReplyParser::IntegratedMeasurement2TimeChanged, &configuration_, &Configuration::Integrated2TimeChanged);
    QObject::connect(&reply_parser_, &ReplyParser::NmbMeasurementsIntegrationTime2Changed, this, &MRM::NmbMeasurementsIntegrationTime2Changed);
    QObject::connect(&reply_parser_, &ReplyParser::NmbMeasurementsIntegrationTime2Changed, &configuration_, &Configuration::NumberOfMeasurementsChanged);
    QObject::connect(&reply_parser_, &ReplyParser::InstantenousMeasurementConversionCoeffChanged, this, &MRM::InstantenousMeasurementConversionCoeffChanged);
    QObject::connect(&reply_parser_, &ReplyParser::InstantenousMeasurementConversionCoeffChanged, &configuration_, &Configuration::InstantaneousConversionCoefficientChanged);
    QObject::connect(&reply_parser_, &ReplyParser::InstantenousMeasurementConversionCoeffChanged, this,
                     [=](double coeff) { inst_conversion_coeff_value__ = coeff; inst_conversion_coeff_init_ = true; });
    QObject::connect(&reply_parser_, &ReplyParser::IntegratedMeasurementConversionCoeffChanged, this, &MRM::IntegratedMeasurementConversionCoeffChanged);
    QObject::connect(&reply_parser_, &ReplyParser::IntegratedMeasurementConversionCoeffChanged, &configuration_, &Configuration::IntegratedConversionCoefficientChanged);
    QObject::connect(&reply_parser_, &ReplyParser::IntegratedMeasurementThresholdLevelChanged, this, &MRM::IntegratedMeasurementThresholdLevelChanged);
    QObject::connect(&reply_parser_, &ReplyParser::IntegratedMeasurementThresholdLevelChanged, &configuration_, &Configuration::IntegratedThresholdChanged);
    QObject::connect(&reply_parser_, &ReplyParser::WarningThreshold1Changed, &configuration_, &Configuration::ThresholdAChanged);
    QObject::connect(&reply_parser_, &ReplyParser::WarningThreshold2Changed, &configuration_, &Configuration::ThresholdBChanged);
    QObject::connect(&reply_parser_, &ReplyParser::WarningThreshold3Changed, &configuration_, &Configuration::ThresholdCChanged);
    QObject::connect(&reply_parser_, &ReplyParser::WarningThreshold1Changed, this, [=](double ampere) {
        if (inst_conversion_coeff_init_) emit(WarningThreshold1Changed(ampere * inst_conversion_coeff_value__)); });
    QObject::connect(&reply_parser_, &ReplyParser::WarningThreshold2Changed, this, [=](double ampere) {
        if (inst_conversion_coeff_init_) emit(WarningThreshold2Changed(ampere * inst_conversion_coeff_value__)); });
    QObject::connect(&reply_parser_, &ReplyParser::WarningThreshold3Changed, this, [=](double ampere) {
        if (inst_conversion_coeff_init_) emit(WarningThreshold3Changed(ampere * inst_conversion_coeff_value__)); });
    QObject::connect(&reply_parser_, &ReplyParser::BaliseError, this, [this](QString msg) { this->error_message_ = msg; });
    QObject::connect(&reply_parser_, &ReplyParser::ReplyError, this, [this](QString msg) { this->error_message_ = msg; });
}

/*
void MRM::ConfigureMeasurement(const SeanceConfig& config) {
    try {
        measurement_params_.dossier = static_cast<int>(config.dossier());
        measurement_params_.tdj_debit = repo_->GetTopDeJourDosimetry(QDateTime::currentDateTime().date()).GetDebitMean();
        measurement_params_.ref_debit = repo_->GetDefaults().GetDRef();
        measurement_params_.pat_debit = static_cast<double>(config.debit()) * measurement_params_.ref_debit / measurement_params_.tdj_debit;
        SetActiveDossier(measurement_params_.dossier);
    }
    catch (std::exception& exc) {
        measurement_params_.dossier = 0;
        QString msg = "Failed configuring measurement\n" + QString(exc.what());
        qWarning() << "MRM::ConfigureMeasurement " << msg;
        emit(DisplayError(msg));
    }
}
*/

void MRM::SendCheckConfiguration() {
    qDebug() << "MRM::SendCheckConfiguration";
     command_list_.push_back(command_creator_.EnableRemote(false));
     command_list_.push_back(command_creator_.EnableReply());
//     command_list_.push_back(command_creator_.SetDate(QDateTime::currentDateTime()));
     command_list_.push_back(command_creator_.ReadNumericParameters());
     command_list_.push_back(command_creator_.ReadModeParameters());
     emit(CommandAdded());
}

void MRM::SendConfiguration() {
//    command_list_.push_back(command_creator_.DisableReply());
    command_list_.push_back(command_creator_.EnableRemote(false));
    command_list_.push_back(command_creator_.StopMeasurement(false));
    command_list_.push_back(command_creator_.EnableReply());
    if (configuration_.IsInitialised()) {
        if (!configuration_.ModeFunctionnementCorrect()) command_list_.push_back(command_creator_.SetModeFunctionnement(configuration_.GetCorrectModeFunctionnement()));
        if (!configuration_.PreAmpliTypeCorrect()) command_list_.push_back(command_creator_.SetPreAmpliType(configuration_.GetCorrectPreAmpliType()));
        if (!configuration_.PreAmpliGammeCorrect()) command_list_.push_back(command_creator_.SetPreAmpliGamme(configuration_.GetCorrectPreAmpliGamme()));
        if (!configuration_.InstantaneousLAMCorrect()) command_list_.push_back(command_creator_.SetEmitInstanenousMeasurement(configuration_.GetCorrectInstantaneousLAM()));
        if (!configuration_.Integrated1LAMCorrect()) command_list_.push_back(command_creator_.SetEmitIntegrated1Measurement(configuration_.GetCorrectIntegrated1LAM()));
        if (!configuration_.Integrated2LAMCorrect()) command_list_.push_back(command_creator_.SetEmitIntegrated2Measurement(configuration_.GetCorrectIntegrated2LAM()));
        if (!configuration_.IncludeDateInMeasurementCorrect()) command_list_.push_back(command_creator_.SetIncludeDateInReply(configuration_.GetCorrectIncludeDateInMeasurement()));
        if (!configuration_.IncludeRawInMeasurementCorrect()) command_list_.push_back(command_creator_.SetIncludeRawMeasurementInReply(configuration_.GetCorrectIncludeRawInMeasurement()));
        if (!configuration_.InstantaneousElementsCorrect()) command_list_.push_back(command_creator_.SetInstantenousMeasurementElements(configuration_.GetCorrectInstantaneousElements()));
        if (!configuration_.InstantaneousTimeCorrect()) command_list_.push_back(command_creator_.SetInstantenousMeasurementTime(configuration_.GetCorrectInstantaneousTime()));
        if (!configuration_.Integrated1TimeCorrect()) command_list_.push_back(command_creator_.SetIntegratedMeasurement1Time(configuration_.GetCorrectIntegrated1Time()));
        if (!configuration_.Integrated2TimeCorrect()) command_list_.push_back(command_creator_.SetIntegratedMeasurement2Time(configuration_.GetCorrectIntegrated2Time()));
        if (!configuration_.NumberOfMeasurementsCorrect()) command_list_.push_back(command_creator_.SetNmbMeasurementsIntegrationTime2(configuration_.GetCorrectNumberOfMeasurements()));
        if (!configuration_.InstantaneousConversionCoefficientCorrect()) command_list_.push_back(command_creator_.SetInstantenousMeasurementConversionCoefficient(configuration_.GetCorrectInstantaneousConversionCoefficient()));
        if (!configuration_.IntegratedConversionCoefficientCorrect()) command_list_.push_back(command_creator_.SetIntegratedMeasurementConversionCoefficient(configuration_.GetCorrectIntegratedConversionCoefficient()));
        if (!configuration_.IntegratedThresholdCorrect()) command_list_.push_back(command_creator_.SetIntegratedMeasurementThresholdLevel(configuration_.GetCorrectIntegratedThreshold()));
        if (!configuration_.ThresholdACorrect()) command_list_.push_back(command_creator_.SetWarningThreshold1(configuration_.GetCorrectThresholdA()));
        if (!configuration_.ThresholdBCorrect()) command_list_.push_back(command_creator_.SetWarningThreshold2(configuration_.GetCorrectThresholdB()));
        if (!configuration_.ThresholdCCorrect()) command_list_.push_back(command_creator_.SetWarningThreshold3(configuration_.GetCorrectThresholdC()));
    }
    command_list_.push_back(command_creator_.SetDate(QDateTime::currentDateTime()));
    command_list_.push_back(command_creator_.ReadNumericParameters());
    command_list_.push_back(command_creator_.ReadModeParameters());
    emit(CommandAdded());
}

void MRM::SendNextCommand() {
    if (command_list_.empty()) {
        return;
    }
    Command cmd(command_list_.takeFirst());
    cmd.Send(client_connection_);
    if (!cmd.wait_confirmation()) {
        emit(CommandSentNoReply());
    } else if (!cmd.wait_reply()) {
        emit(CommandSentConfirmation());
    } else {
        emit(CommandSentReply());
    }
}

void MRM::SetupCommandStateMachine() {
    QState *sCommandSuperState = new QState();
    utils::TimedState *sDisconnected = new utils::TimedState(sCommandSuperState, reconnect_interval_, "Reconnecting");
    QState *sConnected = new QState(sCommandSuperState);
    QState *sIdle = new QState(sConnected);
    QState *sWaitingNoReply = new QState(sConnected);
    QState *sWaitingConfirmation = new QState(sConnected);
    QState *sWaitingReply = new QState(sConnected);
    QState *sDelay = new QState(sConnected);
    QState *sError = new QState(sConnected);

//    QObject::connect(sIdle, &QState::entered, this, [=]() { CommandStateMachine("Idle entered"); });
//    QObject::connect(sIdle, &QState::exited, this, [=]() { CommandStateMachine("Idle exited"); });
//    QObject::connect(sWaitingNoReply, &QState::entered, this, [=]() { CommandStateMachine("WaitingNoReply entered"); });
//    QObject::connect(sWaitingNoReply, &QState::exited, this, [=]() { CommandStateMachine("WaitingNoReply exited"); });
//    QObject::connect(sWaitingConfirmation, &QState::entered, this, [=]() { CommandStateMachine("WaitingConfirmation entered"); });
//    QObject::connect(sWaitingConfirmation, &QState::exited, this, [=]() { CommandStateMachine("WaitingConfirmation exited"); });
//    QObject::connect(sWaitingReply, &QState::entered, this, [=]() { CommandStateMachine("WaitingReply entered"); });
//    QObject::connect(sWaitingReply, &QState::exited, this, [=]() { CommandStateMachine("WaitingReply exited"); });
//    QObject::connect(sError, &QState::entered, this, [=]() { CommandStateMachine("Error entered"); });
//    QObject::connect(sError, &QState::exited, this, [=]() { CommandStateMachine("Error exited"); });
//    QObject::connect(sCommandSuperState, &QState::entered, this, [=]() { CommandStateMachine("CommandSuperState entered"); });
//    QObject::connect(sCommandSuperState, &QState::exited, this, [=]() { CommandStateMachine("CommandSuperState exited"); });

//    QObject::connect(sCommandSuperState, SIGNAL(entered()), this, SLOT(EmptyCommandQueue()));
    sCommandSuperState->addTransition(this, SIGNAL(Abort()), sCommandSuperState);
    sCommandSuperState->addTransition(&reply_parser_, SIGNAL(ReplyError(QString)), sError);
    sCommandSuperState->addTransition(&reply_parser_, SIGNAL(BaliseError(QString)), sError);
    sCommandSuperState->addTransition(this, SIGNAL(CommandError(QString)), sError);
    sCommandSuperState->setInitialState(sDisconnected);

    // Disconnected
    QObject::connect(sDisconnected, &utils::TimedState::SIGNAL_Timeout, this, &MRM::ConnectHardware);
    sDisconnected->addTransition(this, SIGNAL(Connected()), sConnected);

    // Connected
    sConnected->addTransition(this, SIGNAL(Disconnected()), sDisconnected);
    sConnected->setInitialState(sIdle);

    // Idle
    sIdle->addTransition(this, SIGNAL(CommandAdded()), sIdle); // self transition
    sIdle->addTransition(this, SIGNAL(CommandSentNoReply()), sWaitingNoReply);
    sIdle->addTransition(this, SIGNAL(CommandSentConfirmation()), sWaitingConfirmation);
    sIdle->addTransition(this, SIGNAL(CommandSentReply()), sWaitingReply);
    QObject::connect(sIdle, SIGNAL(entered()), this, SLOT(SendNextCommand()));

    QTimer *noReplyTimer = new QTimer(sWaitingNoReply);
    noReplyTimer->setInterval(3000);
    noReplyTimer->setSingleShot(true);
    QObject::connect(sWaitingNoReply, SIGNAL(entered()), noReplyTimer, SLOT(start()));
    QObject::connect(sWaitingNoReply, SIGNAL(exited()), noReplyTimer, SLOT(stop()));
    sWaitingNoReply->addTransition(noReplyTimer, SIGNAL(timeout()), sIdle);

    QTimer *confirmTimer = new QTimer(sWaitingConfirmation);
    confirmTimer->setInterval(2000);
    confirmTimer->setSingleShot(true);
    QObject::connect(sWaitingConfirmation, SIGNAL(entered()), confirmTimer, SLOT(start()));
    QObject::connect(sWaitingConfirmation, SIGNAL(exited()), confirmTimer, SLOT(stop()));
    QObject::connect(confirmTimer, &QTimer::timeout, this, [this]() { this->error_message_ = "Timeout waiting for command confirmation"; });
    sWaitingConfirmation->addTransition(confirmTimer, SIGNAL(timeout()), sError);
    sWaitingConfirmation->addTransition(&reply_parser_, SIGNAL(CommandConfirmed()), sDelay);

    QTimer *replyTimer = new QTimer(sWaitingReply);
    replyTimer->setInterval(3000);
    replyTimer->setSingleShot(true);
    QObject::connect(sWaitingReply, SIGNAL(entered()), replyTimer, SLOT(start()));
    QObject::connect(sWaitingReply, SIGNAL(exited()), replyTimer, SLOT(stop()));
    QObject::connect(replyTimer, &QTimer::timeout, this, [this]() { this->error_message_ = "Timeout waiting for command reply"; });
    sWaitingReply->addTransition(replyTimer, SIGNAL(timeout()), sError);
    sWaitingReply->addTransition(&reply_parser_, SIGNAL(CommandReply()), sDelay);

    QTimer *delayTimer = new QTimer(sDelay);
    delayTimer->setInterval(300);
    delayTimer->setSingleShot(true);
    QObject::connect(sDelay, SIGNAL(entered()), delayTimer, SLOT(start()));
    sDelay->addTransition(delayTimer, SIGNAL(timeout()), sIdle);

    QObject::connect(sError, &QState::entered, this, [=]() { qDebug() << "BroadcastError: " << error_message_; BroadcastError(error_message_); });
    QObject::connect(sError, &QState::exited, this, [=]() { client_connection_->Flush(); });
    QTimer *errorTimer = new QTimer(sError);
    errorTimer->setInterval(3000);
    errorTimer->setSingleShot(true);
    QObject::connect(sError, SIGNAL(entered()), errorTimer, SLOT(start()));
    sError->addTransition(errorTimer, SIGNAL(timeout()), sIdle);

    command_sm_.addState(sCommandSuperState);
    command_sm_.setInitialState(sCommandSuperState);
    command_sm_.start();
}

void MRM::SetupConfiguringStateMachine() {
    QState *sSuperState = new QState();
    QState *sCheckConfiguration = new QState(sSuperState);
    QState *sConfigure = new QState(sSuperState);
    QState *sStartMeasurement = new QState(sSuperState);
    QState *sDone = new QState(sSuperState);
    QState *sError = new QState(sSuperState);

    QObject::connect(sSuperState, &QState::entered, this, [=]() { ConfiguringStateMachine("SuperState entered"); });
    QObject::connect(sSuperState, &QState::exited, this, [=]() { ConfiguringStateMachine("SuperState exited"); });
    QObject::connect(sCheckConfiguration, &QState::entered, this, [=]() { ConfiguringStateMachine("CheckConfiguration entered"); });
    QObject::connect(sCheckConfiguration, &QState::exited, this, [=]() { ConfiguringStateMachine("CheckConfiguration exited"); });
    QObject::connect(sConfigure, &QState::entered, this, [=]() { ConfiguringStateMachine("Configure entered"); });
    QObject::connect(sConfigure, &QState::exited, this, [=]() { ConfiguringStateMachine("Configure exited"); });
    QObject::connect(sStartMeasurement, &QState::entered, this, [=]() { ConfiguringStateMachine("StartMeasurement entered"); });
    QObject::connect(sStartMeasurement, &QState::exited, this, [=]() { ConfiguringStateMachine("StartMeasurement exited"); });
    QObject::connect(sDone, &QState::entered, this, [=]() { ConfiguringStateMachine("Done entered"); });
    QObject::connect(sDone, &QState::exited, this, [=]() { ConfiguringStateMachine("Done exited"); });
    QObject::connect(sError, &QState::entered, this, [=]() { ConfiguringStateMachine("Error entered"); });
    QObject::connect(sError, &QState::exited, this, [=]() { ConfiguringStateMachine("Error exited"); });

    if (settings_->value("misc/configuration_on_startup", "true").toBool()) {
        sSuperState->setInitialState(sCheckConfiguration);
    } else {
        sSuperState->setInitialState(sDone);
    }
    sSuperState->addTransition(this, SIGNAL(StartConfiguration()), sCheckConfiguration);

    // CheckConfiguration State
    QObject::connect(sCheckConfiguration, &QState::entered, this, [=]() { check_config_tries_++;
        if (check_config_tries_ > 5) { check_config_tries_ = 0; error_message_ = "Balise failure. On balise, flip switch \"Reseau\" back and forth and restart software"; emit(AbortConfiguration()); } });
    QObject::connect(sCheckConfiguration, &QState::entered, this, &MRM::StopTick);
    QObject::connect(sCheckConfiguration, &QState::entered, this, &MRM::EmptyCommandQueue);
    QObject::connect(sCheckConfiguration, &QState::entered, &configuration_, &Configuration::ResetStatus);
    QObject::connect(sCheckConfiguration, &QState::entered, this, &MRM::SendCheckConfiguration);
    QObject::connect(sCheckConfiguration, &QState::entered, this, &MRM::ConfiguringInProgress);
    QTimer *checkConfigTimer = new QTimer(sCheckConfiguration);
    checkConfigTimer->setInterval(10000);
    checkConfigTimer->setSingleShot(true);
    QObject::connect(sCheckConfiguration, SIGNAL(entered()), checkConfigTimer, SLOT(start()));
    QObject::connect(sCheckConfiguration, SIGNAL(exited()), checkConfigTimer, SLOT(stop()));
//    sCheckConfiguration->addTransition(checkConfigTimer, SIGNAL(timeout()), sConfigure);
    sCheckConfiguration->addTransition(checkConfigTimer, SIGNAL(timeout()), sCheckConfiguration);
    sCheckConfiguration->addTransition(&configuration_, SIGNAL(Ok()), sStartMeasurement);
    sCheckConfiguration->addTransition(&configuration_, SIGNAL(Wrong()), sConfigure);
    sCheckConfiguration->addTransition(this, SIGNAL(AbortConfiguration()), sError);
//    sCheckConfiguration->addTransition(this, SIGNAL(BroadcastError(QString)), sError);

    // Configure State
    QObject::connect(sConfigure, &QState::entered, this, &MRM::EmptyCommandQueue);
    QObject::connect(sConfigure, &QState::entered, this, &MRM::SendConfiguration);
    QTimer *configureTimer = new QTimer(sConfigure);
    QObject::connect(configureTimer, &QTimer::timeout, this, [=]() { error_message_ = "Configuring timeout (sending configuration)"; });
    configureTimer->setInterval(20000);
    configureTimer->setSingleShot(true);
    QObject::connect(sConfigure, SIGNAL(entered()), configureTimer, SLOT(start()));
    QObject::connect(sConfigure, SIGNAL(exited()), configureTimer, SLOT(stop()));
    sConfigure->addTransition(configureTimer, SIGNAL(timeout()), sError); //
    sConfigure->addTransition(&configuration_, SIGNAL(Ok()), sStartMeasurement);

    // StartMeasurement State
    QObject::connect(sStartMeasurement, &QState::entered, this, [=]() { check_config_tries_ = 0; });
    QObject::connect(sStartMeasurement, &QState::entered, this, &MRM::EmptyCommandQueue);
    QObject::connect(sStartMeasurement, &QState::entered, this, &MRM::StartMeasurement);
    QTimer *startMeasurementTimer = new QTimer(sStartMeasurement);
    startMeasurementTimer->setInterval(20000);
    startMeasurementTimer->setSingleShot(true);
    QObject::connect(sStartMeasurement, SIGNAL(entered()), startMeasurementTimer, SLOT(start()));
    QObject::connect(sStartMeasurement, &QState::exited, startMeasurementTimer, &QTimer::stop);
    QObject::connect(startMeasurementTimer, &QTimer::timeout, this, [=]() { error_message_ = "Configuring timeout (starting measurement)"; });
    sStartMeasurement->addTransition(&reply_parser_, SIGNAL(InstantenousMeasurementChanged(InstantenousMeasurement)), sDone);
    sStartMeasurement->addTransition(startMeasurementTimer, SIGNAL(timeout()), sError);

    // Done State
    QObject::connect(sDone, SIGNAL(entered()), this, SLOT(StartTick()));
    QObject::connect(sDone, SIGNAL(entered()), this, SIGNAL(ConfiguringDone()));
    sDone->addTransition(&configuration_, SIGNAL(Wrong()), sError); // TODO: Write error_message_ = "Configuration Changed"

    // Error State
    QObject::connect(sError, &QState::entered, this, &MRM::StopTick);
    QObject::connect(sError, &QState::entered, this, &MRM::ConfiguringError);
    QObject::connect(sError, &QState::entered, [=]() { emit DisplayError(error_message_); });

    configuring_sm_.addState(sSuperState);
    configuring_sm_.setInitialState(sSuperState);
    configuring_sm_.start();
}

void MRM::SetupMeasurementStateMachine() {
    QState *sIdle = new QState();
    QState *sMeasurement = new QState();
    QState *sStart = new QState(sMeasurement);
    QState *sInProgress = new QState(sMeasurement);
    QState *sStop = new QState(sMeasurement);
    QState *sDelay = new QState(sStop);

    QState *sSave = new QState(sStop);
    QState *sError = new QState();

//    QObject::connect(sMeasurement, &QState::entered, this, [=]() { MeasurementStateMachine("MeasurementState entered"); });
//    QObject::connect(sMeasurement, &QState::exited, this, [=]() { MeasurementStateMachine("MeasurementState exited"); });
//    QObject::connect(sIdle, &QState::entered, this, [=]() { MeasurementStateMachine("Idle entered"); });
//    QObject::connect(sIdle, &QState::exited, this, [=]() { MeasurementStateMachine("Idle exited"); });
//    QObject::connect(sStart, &QState::entered, this, [=]() { MeasurementStateMachine("Start entered"); });
//    QObject::connect(sStart, &QState::exited, this, [=]() { MeasurementStateMachine("Start exited"); });
//    QObject::connect(sStop, &QState::entered, this, [=]() { MeasurementStateMachine("Stop entered"); });
//    QObject::connect(sStop, &QState::exited, this, [=]() { MeasurementStateMachine("Stop exited"); });
//    QObject::connect(sSave, &QState::entered, this, [=]() { MeasurementStateMachine("Save entered"); });
//    QObject::connect(sSave, &QState::exited, this, [=]() { MeasurementStateMachine("Save exited"); });
//    QObject::connect(sDelay, &QState::entered, this, [=]() { MeasurementStateMachine("Delay entered"); });
//    QObject::connect(sDelay, &QState::exited, this, [=]() { MeasurementStateMachine("Delay exited"); });
//    QObject::connect(sError, &QState::entered, this, [=]() { MeasurementStateMachine("Error entered"); });
//    QObject::connect(sError, &QState::exited, this, [=]() { MeasurementStateMachine("Error exited"); });

    // IDLE
    sIdle->addTransition(this, SIGNAL(StartMeasurementSM()), sStart);
    if (continuous_save_) {
        QTimer *pushDbTimer = new QTimer(sIdle);
        pushDbTimer->setInterval(60000);
        pushDbTimer->setSingleShot(false);
        QObject::connect(sIdle, SIGNAL(entered()), pushDbTimer, SLOT(start()));
        QObject::connect(sIdle, SIGNAL(exited()), pushDbTimer, SLOT(stop()));
        QObject::connect(pushDbTimer, SIGNAL(timeout()), this, SLOT(PushMeasurementsToDb()));
    }

    // MEASUREMENT SUPER STATE
    sMeasurement->setInitialState(sStart);
    QObject::connect(this, SIGNAL(StartConfiguration()), this, SIGNAL(AbortMeasurementSM()));
    sMeasurement->addTransition(this, SIGNAL(AbortMeasurementSM()), sIdle);

    // START STATE
    QObject::connect(sStart, &QState::entered, this, [=]() { try { dose_measurement_params_.start = ring_buffer_.CurrentTime().addSecs(-2); }
                                                                    catch(std::exception& exc) { emit AbortMeasurementSM();
                                                                                                 emit DisplayCritical("No integrated measurements available"); } });
    sStart->addTransition(sStart, SIGNAL(entered()), sInProgress);

    // INPROGRESS STATE
    QTimer *inProgressTimer = new QTimer(sInProgress);
    QObject::connect(inProgressTimer, &QTimer::timeout, this, [=]() { error_message_ = "Measurement timeout after 10 min"; });
    inProgressTimer->setInterval(1000 * 10 * 60); // 10 min measurements is max
    inProgressTimer->setSingleShot(true);
    QObject::connect(sInProgress, SIGNAL(entered()), inProgressTimer, SLOT(start()));
    QObject::connect(sInProgress, &QState::exited, inProgressTimer, &QTimer::stop);
    sInProgress->addTransition(inProgressTimer, SIGNAL(timeout()), sStop);
    sInProgress->addTransition(this, SIGNAL(StopMeasurementSM()), sStop);
    QObject::connect(sInProgress, &QState::entered, this, &MRM::MeasurementInProgress);

    // STOP SUPERSTATE
    sStop->setInitialState(sDelay);
    QTimer *delayTimer = new QTimer(sDelay);
    delayTimer->setInterval(3000);
    delayTimer->setSingleShot(true);
    QObject::connect(sDelay, SIGNAL(entered()), delayTimer, SLOT(start()));
    sDelay->addTransition(delayTimer, SIGNAL(timeout()), sSave);
    // Waiting for the next integrated measurement to arrive, then stop
    sDelay->addTransition(this, &MRM::IntegratedMeasurement1Changed, sSave);
    QObject::connect(sDelay, &QState::exited, this, [=]() { dose_measurement_params_.stop = ring_buffer_.CurrentTime(); });
    QObject::connect(sDelay, &QState::exited, this, &MRM::MeasurementStopped);

    QObject::connect(sSave, &QState::entered, this, &MRM::BroadcastMeasurement);
    sSave->addTransition(sSave, &QState::entered, sIdle);

    // Error state
    QObject::connect(sError, &QState::entered, [=]() { emit DisplayError(error_message_); });
    sError->addTransition(sError, SIGNAL(entered()), sIdle);

    measurement_sm_.addState(sIdle);
    measurement_sm_.addState(sMeasurement);
    measurement_sm_.addState(sError);
    measurement_sm_.setInitialState(sIdle);
    measurement_sm_.start();
}

void MRM::SetupReadBufferStateMachine() {
    QState *sIdle = new QState();
    QState *sReadBuffer = new QState();
    QState *sError = new QState();

//    QObject::connect(sIdle, &QState::entered, this, [=]() { ReadBufferStateMachine("Idle entered"); });
//    QObject::connect(sIdle, &QState::exited, this, [=]() { ReadBufferStateMachine("Idle exited"); });
//    QObject::connect(sReadBuffer, &QState::entered, this, [=]() { ReadBufferStateMachine("ReadBuffer entered"); });
//    QObject::connect(sReadBuffer, &QState::exited, this, [=]() { ReadBufferStateMachine("ReadBuffer exited"); });
//    QObject::connect(sError, &QState::entered, this, [=]() { ReadBufferStateMachine("Error entered"); });
//    QObject::connect(sError, &QState::exited, this, [=]() { ReadBufferStateMachine("Error exited"); });

    // IDLE
    QObject::connect(sIdle, &QState::entered, this, [=]() { n_buffer_read_tries_ = 0; });
    sIdle->addTransition(&ring_buffer_, SIGNAL(IntegratedChargeDecreased()), sReadBuffer);

    // READ BUFFER
    QObject::connect(sReadBuffer, &QState::entered, this, [=]() { n_buffer_read_tries_++; });
    QObject::connect(sReadBuffer, &QState::entered, this, [=]() { if (n_buffer_read_tries_ <= 10) ReadBufferIntegratedMeasurement1();
                                                                  else emit(FailedSecureReadIntegratedBuffer()); });
    sReadBuffer->addTransition(this, SIGNAL(FailedSecureReadIntegratedBuffer()), sError);

    QTimer *readBufferTimer = new QTimer(sReadBuffer);
    readBufferTimer->setInterval(2000);
    readBufferTimer->setSingleShot(true);
    QObject::connect(sReadBuffer, SIGNAL(entered()), readBufferTimer, SLOT(start()));
    QObject::connect(sReadBuffer, SIGNAL(exited()), readBufferTimer, SLOT(stop()));
    sReadBuffer->addTransition(readBufferTimer, SIGNAL(timeout()), sReadBuffer);
    sReadBuffer->addTransition(this, SIGNAL(BufferIntegratedMeasurement1Changed(IntegratedMeasurementBuffer)), sIdle);

    // ERROR
    QObject::connect(sError, &QState::entered, this, [=]() { emit(DisplayWarning(QString("Failed reading integrated buffer"))); });
    sError->addTransition(sError, SIGNAL(entered()), sIdle);

    readBuffer_sm_.addState(sIdle);
    readBuffer_sm_.addState(sReadBuffer);
    readBuffer_sm_.addState(sError);
    readBuffer_sm_.setInitialState(sIdle);
    readBuffer_sm_.start();
}

void MRM::SetupMeasurementCheckTimer() {
    measurement_check_timer_.setInterval(15000);
    measurement_check_timer_.setSingleShot(false);
    measurement_check_timer_.start();
    QObject::connect(this, SIGNAL(IntegratedMeasurement1Changed(IntegratedMeasurement)),
                     &measurement_check_timer_, SLOT(start()));
    QObject::connect(&measurement_check_timer_,  &QTimer::timeout,
                     this, [=]() { emit DisplayWarning("Réinitialisation du controleur en cours"); });
    QObject::connect(&measurement_check_timer_,  &QTimer::timeout,
                     this, [=]() { emit StartConfiguration(); });

}

double MRM::GetIntegratedCharge(QDateTime time1, QDateTime time2) {
    try {
        return ring_buffer_.GetIntegratedCharge(time1, time2);
    }
    catch (std::exception& exc) {
        qDebug() << "MRM::GetIntegratedCharge Exception thrown " << exc.what();
        QString msg = "Could not calculate integrated charge between " + time1.toString() + " and " + time2.toString();
        emit(DisplayError(msg));
        return 0.0;
    }
}

void MRM::BroadcastMeasurement() {
    qDebug() << "MRM::BroadcastMeasurement: start time " << dose_measurement_params_.start << " stop time " << dose_measurement_params_.stop;
    const double integrated_charge = GetIntegratedCharge(dose_measurement_params_.start, dose_measurement_params_.stop);
    qDebug() << "MRM::BroadcastMeasurement Integrated charge " << integrated_charge;
    emit(MeasuredCharge(integrated_charge));
}

void MRM::AddCommand(Command command, bool force) {
    if (static_cast<int>(command_list_.size()) <= 1 || force) {
        command_list_.push_back(command);
        emit(CommandAdded());
    } else {
        qWarning() << "MRM::AddCommand There are already" << command_list_.size() << " commands in queue, skipping this one";
    }
}

void MRM::ForceAddCommand(Command command) {
    command_list_.push_back(command);
    emit(CommandAdded());
}

void MRM::PowerOn() { AddCommand(command_creator_.PowerOn()); }
void MRM::StartMeasurement() { AddCommand(command_creator_.StartMeasurement()); }
void MRM::StopMeasurement() { AddCommand(command_creator_.StopMeasurement(true)); }
void MRM::SetSeparator(QString separator) { AddCommand(command_creator_.SetSeparator(separator)); }
void MRM::SetSeparator() { SetSeparator(cmd_separator_); }
void MRM::EnableRemote(bool wait_confirmation) { AddCommand(command_creator_.EnableRemote(wait_confirmation)); }
void MRM::EnableLocal() { AddCommand(command_creator_.EnableLocal()); }
void MRM::EnableReply() { AddCommand(command_creator_.EnableReply()); }
void MRM::DisableReply() { AddCommand(command_creator_.DisableReply()); }

void MRM::SetModeFunctionnement(ModeFunctionnement mode) { AddCommand(command_creator_.SetModeFunctionnement(mode)); }
void MRM::SetPreAmpliType(PreAmpliType type) { AddCommand(command_creator_.SetPreAmpliType(type)); }
void MRM::SetPreAmpliGamme(PreAmpliGamme gamme) { AddCommand(command_creator_.SetPreAmpliGamme(gamme)); }
void MRM::SetEmitInstanenousMeasurement(bool yes) { AddCommand(command_creator_.SetEmitInstanenousMeasurement(yes)); }
void MRM::SetEmitIntegrated1Measurement(bool yes) { AddCommand(command_creator_.SetEmitIntegrated1Measurement(yes)); }
void MRM::SetEmitIntegrated2Measurement(bool yes) { AddCommand(command_creator_.SetEmitIntegrated2Measurement(yes)); }
void MRM::SetIncludeDateInReply(bool yes) { AddCommand(command_creator_.SetIncludeDateInReply(yes)); }
void MRM::SetIncludeRawMeasurementInReply(bool yes) { AddCommand(command_creator_.SetIncludeRawMeasurementInReply(yes)); }

void MRM::SetInstantenousMeasurementTime(int seconds) { AddCommand(command_creator_.SetInstantenousMeasurementTime(seconds)); }
void MRM::SetInstantenousMeasurementElements(int elements) { AddCommand(command_creator_.SetInstantenousMeasurementElements(elements)); }
void MRM::SetNmbMeasurementsBelowThresholdBeforeAlarmReset(int measurements) { AddCommand(command_creator_.SetNmbMeasurementsBelowThresholdBeforeAlarmReset(measurements)); }
void MRM::SetIntegratedMeasurement1Time(int minutes) { AddCommand(command_creator_.SetIntegratedMeasurement1Time(minutes)); }
void MRM::SetIntegratedMeasurement2Time(int hours) { AddCommand(command_creator_.SetIntegratedMeasurement2Time(hours)); }
void MRM::SetNmbMeasurementsIntegrationTime2(int measurements) { AddCommand(command_creator_.SetNmbMeasurementsIntegrationTime2(measurements)); }
void MRM::SetInstantenousMeasurementConversionCoefficient(double coeff) { AddCommand(command_creator_.SetInstantenousMeasurementConversionCoefficient(coeff)); }
void MRM::SetIntegratedMeasurementConversionCoefficient(double coeff) { AddCommand(command_creator_.SetIntegratedMeasurementConversionCoefficient(coeff)); }
void MRM::SetIntegratedMeasurementThresholdLevel(double ampere) { AddCommand(command_creator_.SetIntegratedMeasurementThresholdLevel(ampere)); }
void MRM::SetWarningThreshold1(double conv_value) { AddCommand(command_creator_.SetWarningThreshold1(conv_value)); }
void MRM::SetWarningThreshold2(double conv_value) { AddCommand(command_creator_.SetWarningThreshold2(conv_value)); }
void MRM::SetWarningThreshold3(double conv_value) { AddCommand(command_creator_.SetWarningThreshold3(conv_value)); }

void MRM::ReadDate() { AddCommand(command_creator_.ReadDate()); }
void MRM::SetDate(QDateTime timestamp)  { AddCommand(command_creator_.SetDate(timestamp)); }
void MRM::ReadModeParameters() { AddCommand(command_creator_.ReadModeParameters()); }
void MRM::ReadNumericParameters() { AddCommand(command_creator_.ReadNumericParameters()); }
void MRM::ReadThresholdStatus() { AddCommand(command_creator_.ReadThresholdStatus()); }
void MRM::ReadInstantenousMeasurement() { AddCommand(command_creator_.ReadInstantenousMeasurement()); }
void MRM::ReadIntegratedMeasurement1() { AddCommand(command_creator_.ReadIntegratedMeasurement1()); }
void MRM::ReadIntegratedMeasurement2() { AddCommand(command_creator_.ReadIntegratedMeasurement2()); }
void MRM::ReadBufferIntegratedMeasurement1() { qDebug() << "MRM::ReadBufferIntegratedMeasurement1";
                                                  ForceAddCommand(command_creator_.ReadBufferIntegratedMeasurement1()); }
void MRM::ReadBufferIntegratedMeasurement2() { AddCommand(command_creator_.ReadBufferIntegratedMeasurement2()); }

bool MRM::BackgroundLevel(double dose_sv) const {
    return (dose_sv <= background_level_);
}

void MRM::HandleDetectedXRay(InstantenousMeasurement peak_value) {
    double integrated_charge(0.0);
    try {
        QDateTime fromTime = peak_value.timestamp().addSecs(-2);
        QDateTime toTime = peak_value.timestamp();
        if (ring_buffer_.IntegratedChargeExist(toTime.addSecs(1))) {
            toTime = toTime.addSecs(1);
        }
        integrated_charge = ring_buffer_.GetIntegratedCharge(fromTime, toTime);
    }
    catch (std::exception& exc) {
        qWarning() << "MRM::HandleDetectedXRay Could not get integrated charge: " << exc.what();
    }
    qDebug() << "XRay: " << peak_value.timestamp().toString() << " "
             << peak_value.conversed_value() << " " << integrated_charge;
    emit XRayDetected(peak_value.conversed_value(), integrated_charge);
}

void MRM::ConfigureTick() {
    qDebug() << "MRM::ConfigureTick";
    tick_timer_.setInterval(1000);
    tick_timer_.setSingleShot(false);
    QObject::connect(&tick_timer_, &QTimer::timeout, this, &MRM::ExecuteTick);
}

void MRM::StartTick() {
    tick_timer_.start();
}

void MRM::StopTick() {
    tick_timer_.stop();
}

void MRM::ExecuteTick() {
    ReadIntegratedMeasurement1();
}

void MRM::AddIntegratedToRingBuffer(IntegratedMeasurement m) {
    try {
        ring_buffer_.AddIntegratedValue(m);
    }
    catch (std::exception& exc) {
        qDebug() << "MRM::AddIntegratedToRingBuffer Exception: " << exc.what();
        emit(DisplayWarning(QString(exc.what())));
    }
}

void MRM::AddBufferedToRingBuffer(IntegratedMeasurementBuffer b) {
    try {
        ring_buffer_.AddBufferIntegratedValue(b);
    }
    catch (std::exception& exc) {
        qDebug() << "MRM::AddBufferedToRingBuffer Exception: " << exc.what();
        emit(DisplayWarning(QString(exc.what())));
    }
}

void MRM::PushMeasurementsToDb() {
    qDebug() << "PushMeasurementsToDb";
    try {
        CheckDbConnection("");
        repo_->SaveBaliseInstantaneousLevels(instantaneous_values_);
        instantaneous_values_.clear();
        repo_->SaveBaliseIntegratedLevels(integrated_values_);
        integrated_values_.clear();
        repo_->SaveBaliseBufferLevels(buffer_values_);
        buffer_values_.clear();
    }
    catch (std::exception& exc) {
        qDebug() << "MRM::PushMeasurementsToDb Exception: " << exc.what();
        emit(DisplayWarning("Failed saving radiation level to database"));
    }
}

void MRM::CheckDbConnection(QString message) {
    if (!repo_->CheckConnection()) {
        throw std::runtime_error(message.toStdString());
    }
}
