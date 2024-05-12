#ifndef SERIALPORTWRITER_H
#define SERIALPORTWRITER_H

#include <QtSerialPort/QSerialPort>

#include <QTextStream>
#include <QTimer>
#include <QByteArray>
#include <QObject>
#include <QSettings>

class SerialPortWriter : public QObject
{
    Q_OBJECT

public:
    explicit SerialPortWriter(QSettings& settings);
    ~SerialPortWriter();

    bool Open();
    void Write(const QByteArray &data);
    void Abort();

private slots:
    void CloseIfOpen();
    void HandleBytesWritten(qint64 bytes);
    void HandleTimeout();
    void HandleError(QSerialPort::SerialPortError error);

signals:
    void DataWritten();
    void WriteError(QString error);

private:
    QSerialPort     serial_port_;
    QByteArray      data_;
    qint64          bytes_written_;
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
