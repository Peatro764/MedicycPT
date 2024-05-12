#include "NJ1Sim.h"
#include "ui_NJ1Sim.h"
#include <unistd.h>
#include <iomanip>
#include <sstream>
#include <QDebug>
#include <QStandardPaths>

//#include "SeancePacket.eh"
#include "Conversions.h"

namespace nj1 {

NJ1Sim::NJ1Sim(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::NJ1Sim)
{
    ui_->setupUi(this);

    if (tcp_server_.listen(QHostAddress::Any, 49152)) {
        ui_->plainTextEdit_log->appendPlainText("Listening for connections");
    } else {
        ui_->plainTextEdit_log->appendPlainText("Failed listening for connections");
    }
    connect(&tcp_server_, &QTcpServer::newConnection, this, &NJ1Sim::NewConnection);
    connect(ui_->spinBox_deltaT, QOverload<int>::of(&QSpinBox::valueChanged),
        [=](int value){ timer_.setInterval(value); });

    timer_.setSingleShot(false);
    timer_.setInterval(ui_->spinBox_deltaT->value());
    QObject::connect(&timer_, &QTimer::timeout, this, &NJ1Sim::Tick);
    SetupStateMachine();
}

NJ1Sim::~NJ1Sim() {
}


void NJ1Sim::SetupStateMachine() {

    // States
    QState *sSuperState = new QState();
    QState *sIdle = new QState(sSuperState);
    QState *sSendSeanceStart = new QState(sSuperState);
    QState *sWaitForSeanceStartAcknowledgement = new QState(sSuperState);
    QState *sDelivery = new QState(sSuperState);
    QState *sSendSeanceFinish = new QState(sSuperState);
    QState *sWaitForSeanceFinishAcknowledgement = new QState(sSuperState);

    QObject::connect(sIdle, &QState::entered, this, [=]() { StateMachineMessage("Idle"); });
    QObject::connect(sSendSeanceStart, &QState::entered, this, [=]() { StateMachineMessage("SendSeanceStart"); });
    QObject::connect(sWaitForSeanceStartAcknowledgement, &QState::entered, this, [=]() { StateMachineMessage("WaitForSeanceStartAck"); });
    QObject::connect(sDelivery, &QState::entered, this, [=]() { StateMachineMessage("Delivery"); });
    QObject::connect(sSendSeanceFinish, &QState::entered, this, [=]() { StateMachineMessage("SendSeanceFinish"); });
    QObject::connect(sWaitForSeanceFinishAcknowledgement, &QState::entered, this, [=]() { StateMachineMessage("WaitForSeanceFinishAck"); });

    // SuperState
    sSuperState->setInitialState(sIdle);

    // Idle
    sIdle->addTransition(ui_->pushButton_beam_on, &QPushButton::clicked,  sSendSeanceStart);

    // SendSeanceStart
    connect(sSendSeanceStart, &QState::entered, this, &NJ1Sim::SendSeanceStart);
    sSendSeanceStart->addTransition(sSendSeanceStart, &QState::entered, sWaitForSeanceStartAcknowledgement);

    // WaitForSeanceStartAcknowledgement
    QTimer *waitTimer1 = new QTimer(sWaitForSeanceStartAcknowledgement);
    waitTimer1->setInterval(3000);
    waitTimer1->setSingleShot(true);
    QObject::connect(sWaitForSeanceStartAcknowledgement, SIGNAL(entered()), waitTimer1, SLOT(start()));
    QObject::connect(sWaitForSeanceStartAcknowledgement, SIGNAL(exited()), waitTimer1, SLOT(stop()));
    sWaitForSeanceStartAcknowledgement->addTransition(waitTimer1, SIGNAL(timeout()), sIdle);
    sWaitForSeanceStartAcknowledgement->addTransition(this, &NJ1Sim::SIGNAL_SeanceStartAcknowledgementReceived, sDelivery);

    // Delivery
    connect(sDelivery, &QState::entered, this, [&]() { timer_.start(); });
    connect(sDelivery, &QState::exited, this, [&]() { timer_.stop(); });
    sDelivery->addTransition(ui_->pushButton_beam_off, &QPushButton::clicked, sSendSeanceFinish);
    sDelivery->addTransition(ui_->pushButton_interrupt , &QPushButton::clicked, sIdle);
    sDelivery->addTransition(this, &NJ1Sim::SIGNAL_DoseDelivered, sSendSeanceFinish);

    // SendSeanceFinish
    connect(sSendSeanceFinish, &QState::entered, this, &NJ1Sim::SendSeanceFinish);
    sSendSeanceFinish->addTransition(sSendSeanceFinish, &QState::entered, sWaitForSeanceFinishAcknowledgement);

    // WaitForSeanceFinishAcknowledgement
    QTimer *waitTimer2 = new QTimer(sWaitForSeanceFinishAcknowledgement);
    waitTimer2->setInterval(10000);
    waitTimer2->setSingleShot(true);
    QObject::connect(sWaitForSeanceFinishAcknowledgement, SIGNAL(entered()), waitTimer2, SLOT(start()));
    QObject::connect(sWaitForSeanceFinishAcknowledgement, SIGNAL(exited()), waitTimer2, SLOT(stop()));
    sWaitForSeanceFinishAcknowledgement->addTransition(waitTimer2, SIGNAL(timeout()), sIdle);
    sWaitForSeanceFinishAcknowledgement->addTransition(this, &NJ1Sim::SIGNAL_SeanceFinishAcknowledgementReceived, sIdle);

    sm_.addState(sSuperState);
    sm_.setInitialState(sSuperState);
    sm_.start();
}


void NJ1Sim::Tick() {
    UpdateDeliveryParameters();
    n_packets_++;
    SendSeancePacket(NJ1_COMMAND_TYPE::SEANCE_INPROGRESS);
    UpdateUMsOnGraph();
    if (um1_ >= static_cast<int>(config_.mu_des()) || um2_ >= static_cast<int>(config_.mu_des())) {
        emit (SIGNAL_DoseDelivered());
    }
}

void NJ1Sim::UpdateDeliveryParameters() {
    duration_ += std::max(ui_->spinBox_deltaT->value(), 1);
    um1_ += std::max(ui_->spinBox_umPerSec->value() * ui_->spinBox_deltaT->value() / 1000, 1);
    um2_ += std::max(ui_->spinBox_umPerSec->value() * ui_->spinBox_deltaT->value() / 1000, 1) - 1;
}

void NJ1Sim::SendSeanceStart() {
    n_packets_ = 0;
    SendSeancePacket(NJ1_COMMAND_TYPE::SEANCE_STARTED);
}

void NJ1Sim::SendSeanceFinish() {
    qDebug() << "NPACKETS " << n_packets_;
    SendSeancePacket(NJ1_COMMAND_TYPE::SEANCE_FINISHED);
}

void NJ1Sim::NewConnection() {
    client_socket_ = tcp_server_.nextPendingConnection();
    if (!client_socket_) {
        ui_->plainTextEdit_log->appendPlainText("Failed getting socket for incoming connection");
        return;
    }
    ui_->plainTextEdit_log->appendPlainText("Client connected");
    connect(client_socket_, &QTcpSocket::readyRead, this, &NJ1Sim::ReadAvailableData);
}

void NJ1Sim::ReadAvailableData() {
    auto bytes_available(client_socket_->bytesAvailable());
    QByteArray data(client_socket_->read(bytes_available));
    ui_->plainTextEdit_log->appendPlainText(data);
    ProcessIncomingData(data);
}

void NJ1Sim::SendSeanceConfig(const SeanceConfig& config) {
    QByteArray cmd = AddHeaderInfo(config.ByteArray(), NJ1_COMMAND_TYPE::SEANCE_CONFIG_ACKNOWLEDGEMENT);
    ui_->plainTextEdit_log->appendPlainText("Send seance config");
    client_socket_->write(cmd);
}

void NJ1Sim::SendSeancePacket(NJ1_COMMAND_TYPE type) {
    uint8_t cf9_status(0);
    int32_t i_ct1(0);
    int32_t i_ct2(0);
    if (type == NJ1_COMMAND_TYPE::SEANCE_INPROGRESS) {
        cf9_status = 1;
        i_ct1 = 500;
        i_ct2 = 510;
    } else if (type == NJ1_COMMAND_TYPE::SEANCE_STARTED) {
        cf9_status = 0;
        i_ct1 = 0;
        i_ct2 = 0;
    } else if (type == NJ1_COMMAND_TYPE::SEANCE_FINISHED) {
        cf9_status = 0;
        i_ct1 = 0;
        i_ct2 = 0;
    } else {
     qWarning() << "NJ1Sim::SendSeancePacket unknown type";
    }
    SeancePacket packet(config_.dossier(), config_.seance_id(),
                        cf9_status, duration_, i_ct1, i_ct2, 0, 0, um1_, um2_);
    QByteArray cmd = AddHeaderInfo(packet.ByteArray(), type);
    client_socket_->write(cmd);
}

void NJ1Sim::ProcessIncomingData(QByteArray data) {
    try { // need to catch exceptions here since there is no way for higher levels to catch this exceptions otherwise,
          // since the action is initiated by the sending host
        qDebug() << "NJ1Sim::ProcessIncomingData " << data;

        if (!IsValidReply(data)) {
            ui_->plainTextEdit_log->appendPlainText("NJ1Sim::ProcessIncomingData Not a valid reply");
            return;
        }

        const NJ1_COMMAND_TYPE rcvd_cmd_type = ParseReplyType(data);
        QByteArray core_cmd = RemoveHeaderInfo(data);

        switch (rcvd_cmd_type) {
        case NJ1_COMMAND_TYPE::SEANCE_CONFIG:
            ProcessSeanceConfig(core_cmd);
            break;
        case NJ1_COMMAND_TYPE::SEANCE_STARTED:
            ProcessSeanceStarted(core_cmd);
            break;
        case NJ1_COMMAND_TYPE::SEANCE_FINISHED:
            ProcessSeanceFinished(core_cmd);
            break;
        default:
            ui_->plainTextEdit_log->appendPlainText("An unknown command was received");
            break;
        }
    }
    catch (std::exception& exc) {
        qDebug() << "NJ1Sim::ProcessIncomingData Exception: " << exc.what();
        QString message = QString("An exception was thrown when processing incoming data: ") + exc.what();
    }
}

void NJ1Sim::ProcessSeanceConfig(QByteArray data) {
    config_ = SeanceConfig::UnPack(data);
    ui_->plainTextEdit_log->appendPlainText("SeanceConfig received");
    ui_->lineEdit_uploadSeance_dossier->setText(QString::number(config_.dossier()));
    ui_->lineEdit_uploadseance_name->setText(config_.name());
    ui_->lineEdit_uploadSeance_mu_des->setText(QString::number(config_.mu_des()));
    ui_->lineEdit_uploadSeance_stripper->setText(QString::number(config_.current_stripper()));
    ui_->lineEdit_uploadSeance_duration->setText(QString::number(config_.duration()/1000));

    duration_ = 0;
    um1_ = 0;
    um2_ = 0;
    UpdateUMsOnGraph();

    SendSeanceConfig(config_);
}

void NJ1Sim::UpdateUMsOnGraph() {
    ui_->label_um1->setText(QString::number(um1_));
    ui_->label_um2->setText(QString::number(um2_));
}

void NJ1Sim::ProcessSeanceStarted(QByteArray data) {
    const SeancePacket packet = SeancePacket::UnPack(data);
    ui_->plainTextEdit_log->appendPlainText(QString("Received seance started packet ") + QString::number(packet.seance_id()));
    emit (SIGNAL_SeanceStartAcknowledgementReceived());
}

void NJ1Sim::ProcessSeanceFinished(QByteArray data) {
    const SeancePacket packet = SeancePacket::UnPack(data);
    ui_->plainTextEdit_log->appendPlainText(QString("Received seance finished packet ") + QString::number(packet.seance_id()));
    emit (SIGNAL_SeanceFinishAcknowledgementReceived());
}

QByteArray NJ1Sim::RemoveHeaderInfo(QByteArray cmd) {
    if (cmd.size() <=  NJ1_COMMAND_HEADER_SIZE) {
        throw std::runtime_error("Command from NJ1 does not have the correct size");
    }
    return cmd.mid(NJ1_DATA_BLOCK_START, cmd.size() - NJ1_DATA_BLOCK_START - 3);
}

QByteArray NJ1Sim::AddHeaderInfo(QByteArray cmd, NJ1_COMMAND_TYPE type) {
    QByteArray headers = NJ1_TCP_HEAD +
                         conversions::little_endian::AsByteArray(static_cast<uint16_t>(type), 2) +
                         conversions::little_endian::AsByteArray(static_cast<uint16_t>(0), 2);
    cmd.prepend(headers);
    cmd.append(NJ1_TCP_TAIL);
    return cmd;
}

bool NJ1Sim::IsValidReply(QByteArray data) const {
    if (data.length() < NJ1_COMMAND_HEADER_SIZE) { return false; }
    const QByteArray head = data.left(3);
    const QByteArray tail = data.right(3);
    if (head != NJ1_TCP_HEAD || tail != NJ1_TCP_TAIL) { return false; }
    return true;
}

uint32_t NJ1Sim::ParseErrorCode(QByteArray data) const {
    if (data.size() < NJ1_COMMAND_HEADER_SIZE) { throw std::runtime_error("Cannot parse error code, message too small"); }
    return conversions::little_endian::AsUInt32(data.mid(6, 2));
}

NJ1_COMMAND_TYPE NJ1Sim::ParseReplyType(QByteArray data) const {
    if (data.size() < NJ1_COMMAND_HEADER_SIZE) { throw std::runtime_error("Cannot parse NJ1 reply type, message too small"); }
    return static_cast<NJ1_COMMAND_TYPE>(conversions::little_endian::AsUInt32(data.mid(3, 3)));
}

void NJ1Sim::StateMachineMessage(QString message) {
    ui_->label_sm_state->setText(message);
}

} // namespace nj1


