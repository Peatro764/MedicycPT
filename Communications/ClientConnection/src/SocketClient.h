#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include "ClientConnection.h"

#include <QTcpSocket>
#include <QTextStream>
#include <QDataStream>
#include <QTimer>
#include <QByteArray>
#include <QObject>
#include <QMutex>
#include <QStringList>

class SocketClient : public QObject, public ClientConnection
{
    Q_OBJECT
    Q_INTERFACES(ClientConnection)

public:
    explicit SocketClient(QString ip, int port,
                          int connection_timeout,
                          int read_timeout,
                          int write_timeout,
                          QByteArray cmd_sep);
    ~SocketClient();
    bool Connected() const { return socket_.state() == QTcpSocket::ConnectedState; }

public slots:
    void Connect();
    void Disconnect();
    void Write(const QString& data);
    void Write(const QByteArray& data);
    void Flush();

private slots:
    void HandleSocketError(QAbstractSocket::SocketError error);
    void HandleReadTimeout();
    void HandleConnectionTimeout();
    void HandleWriteTimeout();

    void ReadAvailableData();
    void BytesWritten(qint64 bytes);
    void StateChanged(QAbstractSocket::SocketState state);

signals:
    void DataWritten();
    void DataRead(const QByteArray& data);
    void ReadError(const QString& error);
    void WriteError(const QString& error);
    void PortError(const QString& error);
    void ConnectionEstablished();
    void Disconnected();

private:
    QTcpSocket socket_;
    QString ip_;
    int port_;

    int read_timeout_ms_;
    int write_timeout_ms_;
    int connection_timeout_ms_;
    int bytes_remaining_to_write_;

    QByteArray read_data_buffer_;
    QTimer read_timer_;
    QTimer write_timer_;
    QTimer connection_timer_;

    QByteArray cmd_sep_;
};

#endif
