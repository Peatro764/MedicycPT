#include "NJ1.h"

#include <unistd.h>
#include <iomanip>
#include <sstream>
#include <QDebug>
#include <QStandardPaths>
#include <QSignalTransition>

#include "SeancePacket.h"
#include "Conversions.h"
#include "TimedState.h"

namespace nj1 {

NJ1::NJ1(QString config, bool enable_on_startup)
:       nj1_config_(config),
        settings_(std::unique_ptr<QSettings>(new QSettings(QStandardPaths::locate(QStandardPaths::ConfigLocation, QString("NJ1.ini"), QStandardPaths::LocateFile), QSettings::IniFormat))),
        socket_client_(settings_->value(config + "/ip", "unknown").toString(), settings_->value(config + "/port", "9600").toInt(),
                     settings_->value(config + "/connection_timeout", 3000).toInt(),
                     settings_->value(config + "/read_timeout", 3000).toInt(),
                     settings_->value(config + "/write_timeout", 3000).toInt(),
                     NJ1_TCP_TAIL),
        enable_on_startup_(enable_on_startup)
{
    QObject::connect(&socket_client_, SIGNAL(DataRead(QByteArray)), this, SLOT(ProcessIncomingData(QByteArray)));
//    QObject::connect(&socket_client_, SIGNAL(ReadError(QString)), this, SIGNAL(SIGNAL_IOError(QString)));
//    QObject::connect(&socket_client_, SIGNAL(WriteError(QString)), this, SIGNAL(SIGNAL_IOError(QString)));
//    QObject::connect(&socket_client_, SIGNAL(PortError(QString)), this, SIGNAL(SIGNAL_IOError(QString)));

    // Timer is started by a SeanceStarted packet and stopped by a SeanceFinished packet. The timer is reset
    // by each SeancePacket packet. Ensures that a seance is always finished once it has been started.
    packet_timer_.setSingleShot(true);
    packet_timer_.setInterval(settings_->value(config + "/packet_timer", 3000).toInt());
    QObject::connect(&packet_timer_, &QTimer::timeout, this, &nj1::NJ1::SIGNAL_SeancePacketTimeout);

    // Disconnect if a ping (or other type of packet) has not been received before ping_timeout ms
    ping_timer_.setSingleShot(false);
    ping_timer_.setInterval(settings_->value(config + "/ping_timeout", 3000).toInt());
    QObject::connect(&ping_timer_, &QTimer::timeout, &socket_client_, &SocketClient::Disconnect);
    QObject::connect(this, &NJ1::SIGNAL_ReplyReceived, this, [&]() { ping_timer_.start(); });

    SetupStateMachine();
}

NJ1::~NJ1() {
}

bool NJ1::IsConnected() const {
    return socket_client_.Connected();
}

void NJ1::StateMachineMessage(QString message) {
    qDebug() << QDateTime::currentDateTimeUtc().toString() << "NJ1::StateMachine " << QDateTime::currentDateTime() << " " << message;
}

void NJ1::SetupStateMachine() {

    // Setup statemachine

    // States
    QState *sSuperState = new QState();
        QState *sDisabled = new QState(sSuperState);
        QState *sEnabled = new QState(sSuperState);
            QState *sCheckStatus = new QState(sEnabled);
            utils::TimedState *sDisconnected = new utils::TimedState(sEnabled, settings_->value(nj1_config_ + "/reconnect_interval", 3000).toInt(), "Reconnecting");
            QState *sConnected = new QState(sEnabled);
                QState *sIdle = new QState(sConnected);
                utils::TimedState *sExecute = new utils::TimedState(sConnected, 300, "Wait until sending next command", sIdle);

    // SuperState
    sSuperState->setInitialState(enable_on_startup_ ? sEnabled : sDisabled);

        // Disabled
        sDisabled->addTransition(this, &NJ1::SIGNAL_Enable, sEnabled);

        // Enabled
        sEnabled->setInitialState(sCheckStatus);
        auto tDisable = sEnabled->addTransition(this, &NJ1::SIGNAL_Disable, sDisabled);
        QObject::connect(tDisable, &QSignalTransition::triggered, &socket_client_, &SocketClient::Disconnect);
        auto tConnected = sEnabled->addTransition(&socket_client_, &SocketClient::ConnectionEstablished, sConnected);
        tConnected->setTransitionType(QAbstractTransition::InternalTransition);
        QObject::connect(tConnected, &QSignalTransition::triggered, this, &NJ1::SIGNAL_Connected);

            // CheckStatus
            QObject::connect(sCheckStatus, &QState::entered, &socket_client_, &SocketClient::Connect);
            auto tDisconnectedInCheckStatus = sCheckStatus->addTransition(&socket_client_, &SocketClient::Disconnected, sDisconnected);
            QObject::connect(tDisconnectedInCheckStatus, &QSignalTransition::triggered, this, &NJ1::SIGNAL_Disconnected);

            // Disconnected
            QObject::connect(sDisconnected, &QState::entered, this, [&]() { ping_timer_.stop(); });
            QObject::connect(sDisconnected, &utils::TimedState::SIGNAL_Timeout, &socket_client_, &SocketClient::Connect);

            // Connected
            sConnected->setInitialState(sIdle);
            QObject::connect(sConnected, &QState::entered, this, [&]() { command_queue_.clear(); });
            QObject::connect(sConnected, &QState::entered, this, [&]() { ping_timer_.start(); });
            auto tDisconnected = sConnected->addTransition(&socket_client_, &SocketClient::Disconnected, sDisconnected);
            QObject::connect(tDisconnected, &QSignalTransition::triggered, this, &NJ1::SIGNAL_Disconnected);

                // Idle
                QObject::connect(sIdle, &QState::entered, this, [&]() { if (!command_queue_.empty()) emit(SIGNAL_CommandAdded()); });
                sIdle->addTransition(this, &nj1::NJ1::SIGNAL_CommandAdded, sExecute);

                // Execute
                QObject::connect(sExecute, &QState::entered, this, [&]() { if (!command_queue_.empty()) socket_client_.Write(command_queue_.pop()); });

    PrintStateChanges(sSuperState, "SuperState");
    PrintStateChanges(sEnabled, "Enabled");
    PrintStateChanges(sDisabled, "Disabled");
    PrintStateChanges(sCheckStatus, "CheckStatus");
    PrintStateChanges(sDisconnected, "Disconnected");
    PrintStateChanges(sConnected, "Connected");
//    PrintStateChanges(sIdle, "Idle");
//    PrintStateChanges(sExecute, "Execute");

    sm_.addState(sSuperState);
    sm_.setInitialState(sSuperState);
    sm_.start();
}

// TODO Rewrite threadsafe
void NJ1::AddToQueue(QByteArray cmd) {
    if (command_queue_.size() < MAXQUEUESIZE) {
        command_queue_.push(cmd);
        emit (SIGNAL_CommandAdded());
    } else {
        qWarning() << "NJ1::AddToQueue Max queue size reached, ignoring command";
    }
}

void NJ1::SendPingCommand() {
    AddToQueue(AddHeaderInfo(QByteArray(), NJ1_COMMAND_TYPE::PING));
}

void NJ1::UploadNewSeanceConfig(const SeanceConfig& config) {
    uploaded_seance_config_ = config;
    AddToQueue(AddHeaderInfo(config.ByteArray(), NJ1_COMMAND_TYPE::SEANCE_CONFIG));
}

void NJ1::AcknowledgeSeanceFinish() {
    qDebug() << "NJ1::AckSeanceFinished";
    AddToQueue(AddHeaderInfo(seance_finished_packet_.ByteArray(), NJ1_COMMAND_TYPE::SEANCE_FINISHED));
}

void NJ1::AcknowledgeSeanceStart() {
    qDebug() << "NJ1::AckSeanceStart";
    AddToQueue(AddHeaderInfo(seance_started_packet_.ByteArray(), NJ1_COMMAND_TYPE::SEANCE_STARTED));
}

void NJ1::ProcessIncomingData(QByteArray data) {
    try { // need to catch exceptions here since there is no way for higher levels to catch this exceptions otherwise,
          // since the action is initiated by the sending host

        if (IsValidReply(data)) {
            emit (SIGNAL_ReplyReceived());
        } else {
            qDebug() << "NJ1::ProcessIncomingData Not a valid reply";
            throw std::runtime_error("Not a valid reply");
        }

        const NJ1_COMMAND_TYPE rcvd_cmd_type = ParseReplyType(data);
        QByteArray core_cmd = RemoveHeaderInfo(data);

        switch (rcvd_cmd_type) {
        case NJ1_COMMAND_TYPE::SEANCE_CONFIG:
            ProcessSeanceConfig(core_cmd);
            break;
        case NJ1_COMMAND_TYPE::SEANCE_CONFIG_ACKNOWLEDGEMENT:
            ProcessSeanceConfigAcknowledgement(core_cmd);
            break;
        case NJ1_COMMAND_TYPE::SEANCE_STARTED:
            ProcessSeanceStarted(core_cmd);
            break;
        case NJ1_COMMAND_TYPE::SEANCE_INPROGRESS:
            ProcessSeanceInProgress(core_cmd);
            break;
        case NJ1_COMMAND_TYPE::SEANCE_FINISHED:
            ProcessSeanceFinished(core_cmd);
            break;
        case NJ1_COMMAND_TYPE::SEANCE_ABORTED:
            ProcessSeanceAborted(core_cmd);
            break;
        case NJ1_COMMAND_TYPE::PING:
            SendPingCommand();
            break;
        default:
            ProcessUnknownReply(core_cmd);
            break;
        }
    }
    catch (std::exception& exc) {
        qDebug() << "NJ1::ProcessIncomingData Exception: " << exc.what();
        QString message = QString("An exception was thrown when processing incoming data: ") + exc.what();
        emit(SIGNAL_InterruptCommand(message));
    }
}

void NJ1::ProcessUnknownReply(QByteArray data) {
    QString message = QString("An unknown reply type was received: ") + data;
    qDebug() << "NJ1::ProcessUnknownReply " << message;
    emit(SIGNAL_InterruptCommand(message));
}

void NJ1::ProcessSeanceConfig(QByteArray data) {
    SeanceConfig recv_config = SeanceConfig::UnPack(data);
    std::stringstream ss;
    ss << "Recieved seance config: "  << recv_config;
    qWarning() << "NJ1::ProcessSeanceConfig " << QString::fromStdString(ss.str());
    emit(SIGNAL_SeanceConfig(recv_config));
}

void NJ1::ProcessSeanceConfigAcknowledgement(QByteArray data) {
    SeanceConfig recv_config = SeanceConfig::UnPack(data);
    if (uploaded_seance_config_ == recv_config) {
        emit(SIGNAL_SeanceConfigAckOk());
        qDebug() << "NJ1::ProcessSeanceConfigAcknowledgement seance is identical";
    } else {
        emit(SIGNAL_SeanceConfigAckError());
        std::stringstream ss;
        ss << "Sent: " << uploaded_seance_config_ << " Recv: " << recv_config;
        qWarning() << "NJ1::ProcessSeanceConfigAcknowledgement seance is different: " << QString::fromStdString(ss.str());
    }
}

void NJ1::ProcessSeanceStarted(QByteArray data) {
    qDebug() << "NJ1::ProcessSeanceStarted";
    packet_timer_.start();
    seance_started_packet_ = SeancePacket::UnPack(data);
    emit (SIGNAL_SeanceStartedPacket(seance_started_packet_));
}

void NJ1::ProcessSeanceInProgress(QByteArray data) {
    packet_timer_.start();
    const SeancePacket seance_packet = SeancePacket::UnPack(data);
    emit (SIGNAL_SeancePacket(seance_packet));
}

void NJ1::ProcessSeanceFinished(QByteArray data) {
    qDebug() << "NJ1::ProcessSeanceFinished";
    packet_timer_.stop();
    seance_finished_packet_ = SeancePacket::UnPack(data);
    emit (SIGNAL_SeanceFinishedPacket(seance_finished_packet_));
}

void NJ1::ProcessSeanceAborted(QByteArray data) {
    qDebug() << "NJ1::ProcessSeanceAborted";
    const SeancePacket seance_packet = SeancePacket::UnPack(data);
    emit (SIGNAL_SeanceAbortedPacket(seance_packet));
}

QByteArray NJ1::RemoveHeaderInfo(QByteArray cmd) {
    if (cmd.size() <  NJ1_COMMAND_HEADER_SIZE) {
        throw std::runtime_error("Command from NJ1 does not have the correct size");
    }
    return cmd.mid(NJ1_DATA_BLOCK_START, cmd.size() - NJ1_DATA_BLOCK_START - 3);
}

QByteArray NJ1::AddHeaderInfo(QByteArray cmd, NJ1_COMMAND_TYPE type) const {
    QByteArray headers = NJ1_TCP_HEAD +
                         conversions::little_endian::AsByteArray(static_cast<uint16_t>(type), 2) +
                         conversions::little_endian::AsByteArray(static_cast<uint16_t>(0), 2);
    cmd.prepend(headers);
    cmd.append(NJ1_TCP_TAIL);
    return cmd;
}

bool NJ1::IsValidReply(QByteArray data) const {
    if (data.length() < NJ1_COMMAND_HEADER_SIZE) { return false; }
    const QByteArray head = data.left(3);
    const QByteArray tail = data.right(3);
    if (head != NJ1_TCP_HEAD || tail != NJ1_TCP_TAIL) { return false; }
    return true;
}

uint32_t NJ1::ParseErrorCode(QByteArray data) const {
    if (data.size() < NJ1_COMMAND_HEADER_SIZE) { throw std::runtime_error("Cannot parse error code, message too small"); }
    return conversions::little_endian::AsUInt32(data.mid(5, 2));
}

NJ1_COMMAND_TYPE NJ1::ParseReplyType(QByteArray data) const {
    if (data.size() < NJ1_COMMAND_HEADER_SIZE) { throw std::runtime_error("Cannot parse NJ1 reply type, message too small"); }
    return static_cast<NJ1_COMMAND_TYPE>(conversions::little_endian::AsUInt32(data.mid(3, 2)));
}

void NJ1::PrintStateChanges(QState *state, QString name) {
    QObject::connect(state, &QState::entered, this, [&, name]() { qDebug() << "NJ1 " << QDateTime::currentDateTime() << " ->" << name; });
    QObject::connect(state, &QState::exited, this, [&, name]() { qDebug() << "NJ1 " << QDateTime::currentDateTime() << " <-" << name; });
}

}

