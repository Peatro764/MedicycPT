#ifndef SERIALPORTREADER_H
#define SERIALPORTREADER_H

#include <QtSerialPort/QSerialPort>

#include <QTextStream>
#include <QDataStream>
#include <QTimer>
#include <QByteArray>
#include <QObject>
#include <QSettings>

#include "DataReader.h"

class SerialPortReader : public DataReader
{
    Q_OBJECT

public:
    explicit SerialPortReader(QSettings& settings);
    ~SerialPortReader();

    void Read();
    void Abort();
    bool Open();

private slots:
    void Cleanup();
    void HandleTimeout();
    void HandleError(QSerialPort::SerialPortError error);
    void ReadAvailableData();

signals:
//    void ProgramRead(QString program);
//    void ReadError(QString error);

private:
    QSerialPort     serial_port_;
    QByteArray      data_;
    qint64          bytes_read_;
    QTimer          timer_;

    QString port_;
    qint32 baud_rate_;
    QSerialPort::DataBits data_bits_;
    QSerialPort::Parity parity_;
    QSerialPort::StopBits stop_bits_;
    QSerialPort::FlowControl flow_control_;

    int timeout_ms_;
};

#endif
