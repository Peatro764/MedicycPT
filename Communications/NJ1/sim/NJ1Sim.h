#ifndef NJ1SIM_H
#define NJ1SIM_H

#include <QStateMachine>
#include <QWidget>
#include <QObject>
#include <QString>
#include <QSettings>
#include <QStateMachine>
#include <QDateTime>
#include <memory>
#include <queue>
#include <QTimer>
#include <QTcpServer>
#include <QTcpSocket>

#include "Util.h"
#include "SeanceConfig.h"
#include "SeancePacket.h"

namespace Ui {
class NJ1Sim;
}

namespace nj1 {

const QByteArray NJ1_TCP_HEAD("MTD");
const QByteArray NJ1_TCP_TAIL("DTM");
const int NJ1_COMMAND_HEADER_SIZE(NJ1_TCP_HEAD.size() + 2 + 2 + NJ1_TCP_TAIL.size());
const int NJ1_DATA_BLOCK_START(NJ1_TCP_HEAD.size() + 2 + 2);
const int NJ1_UPLOAD_SEANCE_SIZE(NJ1_COMMAND_HEADER_SIZE + SeanceConfig::SERIALIZED_PACKET_SIZE);
const int NJ1_SEANCE_INFO_SIZE(NJ1_COMMAND_HEADER_SIZE + SeancePacket::SERIALIZED_PACKET_SIZE);

enum class NJ1_COMMAND_TYPE { SEANCE_CONFIG = 0, SEANCE_CONFIG_ACKNOWLEDGEMENT = 1, SEANCE_STARTED = 2, SEANCE_INPROGRESS = 3, SEANCE_FINISHED = 4, SEANCE_ABORTED = 6 };


class NJ1Sim : public QWidget
{
    Q_OBJECT
public:
    explicit NJ1Sim(QWidget *parent = 0);
    ~NJ1Sim();

public slots:

private slots:
    void SendSeanceStart();
    void SendSeanceFinish();
    void NewConnection();
    void ReadAvailableData();
    void ProcessIncomingData(QByteArray data);
    void ProcessSeanceConfig(QByteArray data);
    void ProcessSeanceStarted(QByteArray data);
    void ProcessSeanceFinished(QByteArray data);
    void Tick();
    void UpdateDeliveryParameters();

signals:
    void SIGNAL_SeanceStartAcknowledgementReceived();
    void SIGNAL_SeanceFinishAcknowledgementReceived();
    void SIGNAL_DoseDelivered();

private:
    void SetupStateMachine();
    void UpdateUMsOnGraph();
    void SendSeanceConfig(const SeanceConfig& config);
    void SendSeancePacket(NJ1_COMMAND_TYPE type);
    bool IsValidReply(QByteArray data) const;
    NJ1_COMMAND_TYPE ParseReplyType(QByteArray data) const;
    uint32_t ParseErrorCode(QByteArray data) const;
    QByteArray AddHeaderInfo(QByteArray cmd, NJ1_COMMAND_TYPE type);
    QByteArray RemoveHeaderInfo(QByteArray cmd);
    void StateMachineMessage(QString message);

    int n_packets_ = 0;
    int duration_ = 0;
    int um1_ = 0;
    int um2_ = 0;
    SeanceConfig config_;
    Ui::NJ1Sim *ui_;
    QTcpServer tcp_server_;
    QTcpSocket *client_socket_;
    QTimer timer_;
    QStateMachine sm_;
};

}

#endif
