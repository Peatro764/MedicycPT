#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <QTcpSocket>

#include <QTextStream>
#include <QDataStream>
#include <QTimer>
#include <QByteArray>
#include <QObject>
#include <QMutex>
#include <QStringList>

class SocketClient : public QObject
{
    Q_OBJECT

public:
    explicit SocketClient(QString ip, int port,
                          int connection_timeout, int reconnection_timeout,
                          int read_timeout, int write_timeout,
                          QString cmd_sep,
                          bool reconnect_on_timeout);
    ~SocketClient();
    bool Connected() const { return socket_.state() == QTcpSocket::ConnectedState; }

public slots:
    void Connect();
    void Disconnect() { socket_.abort(); }
    void Write(const QString& data);
    void Write(const QByteArray& data);

private slots:
    void HandleSocketError(QAbstractSocket::SocketError error);
    void HandleReadTimeout();
    void HandleConnectionTimeout();
    void HandleWriteTimeout();

    void ReadAvailableData();
    void BytesWritten(qint64 bytes);
    void StateChanged(QAbstractSocket::SocketState state);

signals:
    void DataRead(const QByteArray& data);
    void DataRead(const QString& data);
    void ReadError(const QString& error);
    void WriteError(const QString& error);
    void SocketError(const QString& error);
    void ConnectionEstablished();
    void Disconnected();
    void DataWritten();

private:
    QTcpSocket socket_;
    QString ip_;
    int port_;

    int read_timeout_ms_;
    int write_timeout_ms_;
    int connection_timeout_ms_;
    int reconnection_timeout_ms_;
    int bytes_remaining_to_write_;

    QString      read_data_buffer_;
    QTimer          read_timer_;
    QTimer          write_timer_;
    QTimer          connection_timer_;
    QTimer          reconnection_timer_;

    QString cmd_sep_;
    bool reconnect_on_timeout_ = false;
};

#endif
