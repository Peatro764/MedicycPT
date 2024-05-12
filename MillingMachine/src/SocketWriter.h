#ifndef SOCKETWRITER_H
#define SOCKETWRITER_H

#include <QTcpSocket>

#include <QTextStream>
#include <QTimer>
#include <QByteArray>
#include <QObject>
#include <QSettings>

class SocketWriter : public QObject
{
    Q_OBJECT

public:
    explicit SocketWriter(QSettings& settings);
    ~SocketWriter();

    void Write(const QByteArray &data);
    void Abort();

private slots:
    void Open();
    void Cleanup();
    void WriteData();
    void ConnectionEstablished();
    void HandleBytesWritten(qint64 bytes);
    void HandleConnectionTimeout();
    void HandleWriteTimeout();
    void HandleError(QAbstractSocket::SocketError error);
    void CloseConnection();

signals:
    void DataWritten();
    void WriteError(QString error);

private:
    QTcpSocket socket_;
    QByteArray      data_;
    qint64          bytes_written_;
    QTimer          write_timer_;
    QTimer          connection_timer_;
    QTimer          close_connection_timer_;

    QString ip_;
    QString port_;
    int write_timeout_ms_;
    int connection_timeout_ms_;
};

#endif
