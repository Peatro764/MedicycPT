#ifndef SERIALCLIENT_H
#define SERIALCLIENT_H

#include "ClientConnection.h"

#include <QtSerialPort/QSerialPort>

#include <QTextStream>
#include <QDataStream>
#include <QTimer>
#include <QByteArray>
#include <QObject>
#include <QMutex>
#include <QStringList>

class SerialClient : public QObject, public ClientConnection
{
    Q_OBJECT
    Q_INTERFACES(ClientConnection)

public:
    SerialClient(QString port, QSerialPort::BaudRate baud_rate, QSerialPort::DataBits data_bits,
                 QSerialPort::Parity parity, QSerialPort::StopBits stop_bits, QSerialPort::FlowControl flow_control,
                 int connection_timeout_ms, int read_timeout_ms, int write_timeout_ms,
                 QByteArray separator);
    ~SerialClient();
    bool Connected() const { return serial_port_.isOpen(); }

public slots:
    void Connect();
    void Disconnect() { serial_port_.close(); }
    void Write(const QString& data);
    void Write(const QByteArray& data);
    void Flush();

private slots:

    void HandleError(QSerialPort::SerialPortError error);
    void HandleReadTimeout();
    void HandleWriteTimeout();

    void ReadAvailableData();
    void BytesWritten(qint64 bytes);

    void CloseIfOpen();

signals:
    void DataWritten();
    void DataRead(const QByteArray& data);
    void ReadError(const QString& error);
    void WriteError(const QString& error);
    void PortError(const QString& error);
    void ConnectionEstablished();
    void Disconnected();

private:
    QSerialPort serial_port_;
    QString port_;
    QSerialPort::BaudRate baud_rate_;
    QSerialPort::DataBits data_bits_;
    QSerialPort::Parity parity_;
    QSerialPort::StopBits stop_bits_;
    QSerialPort::FlowControl flow_control_;

    int connection_timeout_ms_;
    int read_timeout_ms_;
    int write_timeout_ms_;
    qint64 bytes_remaining_to_write_;

    QByteArray      read_data_buffer_;
    QTimer          read_timer_;
    QTimer          write_timer_;
    QTimer          connection_timer_;

    QByteArray separator_;
};

#endif
