#ifndef SOCKETREADER_H
#define SOCKETREADER_H

#include <QTcpSocket>

#include <QTextStream>
#include <QDataStream>
#include <QTimer>
#include <QByteArray>
#include <QObject>
#include <QSettings>

#include "DataReader.h"

class SocketReader : public DataReader
//        , QObject
{
    Q_OBJECT

public:
    explicit SocketReader(QSettings& settings);
    ~SocketReader();

    void Read();
    void Abort();

private slots:
    void Open();
    void Cleanup();
    void ConnectionEstablished();
    void HandleConnectionTimeout();
    void HandleReadTimeout();
    void HandleError(QAbstractSocket::SocketError error);
    void ReadAvailableData();


signals:
//    void ProgramRead(QString program);
//    void ReadError(QString error);

private:
    QTcpSocket socket_;
    QByteArray      data_;
    qint64          bytes_read_;
    QTimer          read_timer_;
    QTimer          connection_timer_;

    QString ip_;
    QString port_;
    int read_timeout_ms_;
    int connection_timeout_ms_;
};

#endif
