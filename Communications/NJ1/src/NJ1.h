#ifndef NJ1_H
#define NJ1_H

#include <QObject>
#include <QString>
#include <QSettings>
#include <QStateMachine>
#include <QDateTime>
#include <memory>
#include <queue>
#include <QTimer>

#include "SocketClient.h"
#include "Util.h"
#include "ThreadSafeQueue.h"
#include "SeanceConfig.h"
#include "SeancePacket.h"

namespace nj1 {

const QByteArray NJ1_TCP_HEAD("MTD");
const QByteArray NJ1_TCP_TAIL("DTM");
const int NJ1_COMMAND_HEADER_SIZE(NJ1_TCP_HEAD.size() + 2 + 2 + NJ1_TCP_TAIL.size());
const int NJ1_DATA_BLOCK_START(NJ1_TCP_HEAD.size() + 2 + 2);
const int NJ1_UPLOAD_SEANCE_SIZE(NJ1_COMMAND_HEADER_SIZE + SeanceConfig::SERIALIZED_PACKET_SIZE);
const int NJ1_SEANCE_INFO_SIZE(NJ1_COMMAND_HEADER_SIZE + SeancePacket::SERIALIZED_PACKET_SIZE);

enum class NJ1_COMMAND_TYPE { SEANCE_CONFIG = 0, SEANCE_CONFIG_ACKNOWLEDGEMENT = 1, SEANCE_STARTED = 2, SEANCE_INPROGRESS = 3, SEANCE_FINISHED = 4, SEANCE_ABORTED = 5, PING = 6 };


class NJ1 : public QObject
{
    Q_OBJECT
public:
    NJ1(QString config, bool enable_on_startup);
    ~NJ1();
    bool IsConnected() const;
    void UploadNewSeanceConfig(const SeanceConfig& config);
    void AcknowledgeSeanceStart();
    void AcknowledgeSeanceFinish();

public slots:    

private slots:
    void ProcessIncomingData(QByteArray data);
    void ProcessSeanceConfig(QByteArray data);
    void ProcessSeanceConfigAcknowledgement(QByteArray data);
    void ProcessSeanceStarted(QByteArray data);
    void ProcessSeanceInProgress(QByteArray data);
    void ProcessSeanceFinished(QByteArray data);
    void ProcessSeanceAborted(QByteArray data);
    void ProcessUnknownReply(QByteArray data);
    void StateMachineMessage(QString message);
    void AddToQueue(QByteArray cmd);
    void SendPingCommand();

signals:
    void SIGNAL_Connected();
    void SIGNAL_Disconnected();
    void SIGNAL_CommandAdded();
    void SIGNAL_ReplyReceived();
    void SIGNAL_InterruptCommand(QString message);
    void SIGNAL_IOError(QString error);

    void SIGNAL_Enable();
    void SIGNAL_Disable();

    void SIGNAL_SeanceConfig(const SeanceConfig& seance_config);
    void SIGNAL_SeanceConfigAckOk();
    void SIGNAL_SeanceConfigAckError();

    void SIGNAL_SeancePacket(const SeancePacket& seance_packet);
    void SIGNAL_SeanceStartedPacket(const SeancePacket& seance_packet);
    void SIGNAL_SeanceFinishedPacket(const SeancePacket& seance_packet);
    void SIGNAL_SeanceAbortedPacket(const SeancePacket& seance_packet);
    void SIGNAL_SeancePacketTimeout();

private:
    void SetupStateMachine();
    void PrintStateChanges(QState *state, QString name);
    bool IsValidReply(QByteArray data) const;
    NJ1_COMMAND_TYPE ParseReplyType(QByteArray data) const;
    uint32_t ParseErrorCode(QByteArray data) const;
    QByteArray AddHeaderInfo(QByteArray cmd, NJ1_COMMAND_TYPE type) const;
    QByteArray RemoveHeaderInfo(QByteArray cmd);

    QString nj1_config_;
    std::unique_ptr<QSettings> settings_;
    SocketClient socket_client_;
    QTimer ping_timer_;
    QTimer packet_timer_;

    SeanceConfig uploaded_seance_config_;
    SeancePacket seance_started_packet_;
    SeancePacket seance_finished_packet_;

    QStateMachine sm_;

    utils::ThreadSafeQueue<QByteArray> command_queue_;
    const size_t MAXQUEUESIZE = 5;
    bool enable_on_startup_;
};

}

#endif
