#include "SerialPortWriter.h"

#include <QDebug>

SerialPortWriter::SerialPortWriter(QSettings& settings)
    : bytes_written_(0)
    , timeout_ms_(1000)

{
    timer_.setSingleShot(true);
    QObject::connect(&serial_port_, &QSerialPort::bytesWritten, this, &SerialPortWriter::HandleBytesWritten);
    QObject::connect(&serial_port_, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
            this, &SerialPortWriter::HandleError);
    QObject::connect(&timer_, &QTimer::timeout, this, &SerialPortWriter::HandleTimeout);

    port_ = settings.value("serial_comm/port", "unknown").toString();
    baud_rate_ = settings.value("serial_comm/baudrate", "9600").toInt();
    data_bits_ = static_cast<QSerialPort::DataBits>(settings.value("serial_comm/databits", "7").toInt());
    parity_ = static_cast<QSerialPort::Parity>(settings.value("serial_comm/parity","0").toInt());
    stop_bits_ = static_cast<QSerialPort::StopBits>(settings.value("serial_comm/stopbits", "1").toInt());
    flow_control_ = static_cast<QSerialPort::FlowControl>(settings.value("serial_comm/flowcontrol", "0").toInt());
    timeout_ms_ = settings.value("serial_comm/timeout", "5000").toInt();
}

SerialPortWriter::~SerialPortWriter()
{
}

void SerialPortWriter::Abort() {
    timer_.stop();
    CloseIfOpen();
}

bool SerialPortWriter::Open() {
    serial_port_.setPortName(port_);
    qDebug() << "SerialPortWriter Opening port " << port_;
    if (serial_port_.isOpen()) {
        qDebug() << "SerialPortWriter Port already opened";
        serial_port_.close();
    }

    if (!serial_port_.open(QIODevice::WriteOnly)) {
        qWarning() << QString("SerialPortWriter::SendProgramOverSerialPort Could not open serial port: ") + serial_port_.errorString();
        serial_port_.close();
        return false;
    }

    if (!serial_port_.setBaudRate(baud_rate_)) {
        qDebug() << "SerialPortWriter::SendProgramOverSerialPort SetBaudRate failed";
        return false;
    } else {
        qDebug() << "SerialPortWriter::SendProgramOverSerialPort BaudRate = " << baud_rate_;
    }
    if (!serial_port_.setDataBits(data_bits_)) {
        qDebug() << "SerialPortWriter::SendProgramOverSerialPort SetDataBits failed";
        return false;
    } else {
        qDebug() << "SerialPortWriter::SendProgramOverSerialPort DataBits = " << data_bits_;
    }
    if (!serial_port_.setParity(parity_)) {
        qDebug() << "SerialPortWriter::SendProgramOverSerialPort SetParity failed";
        return false;
    } else {
        qDebug() << "SerialPortWriter::SendProgramOverSerialPort Parity = " << parity_;
    }
    if (!serial_port_.setStopBits(stop_bits_)) {
        qDebug() << "SerialPortWriter::SendProgramOverSerialPort SetStopBits failed";
        return false;
    } else {
        qDebug() << "SerialPortWriter::SendProgramOverSerialPort StopBits = " << stop_bits_;
    }
    if (!serial_port_.setFlowControl(flow_control_)) {
        qDebug() << "SerialPortWriter::SendProgramOverSerialPort SetFlowControl failed";
        return false;
    } else {
        qDebug() << "SerialPortWriter::SendProgramOverSerialPort FlowControl = " << flow_control_;
    }
    return true;
}

void SerialPortWriter::HandleBytesWritten(qint64 bytes)
{
    qDebug() << "SerialPortWriter::HandleBytesWritten: " << bytes;
    bytes_written_ += bytes;
    if (bytes_written_ == data_.size()) {
        bytes_written_ = 0;
        qDebug() << QObject::tr("Data successfully sent to port %1").arg(serial_port_.portName());
        timer_.stop();
        emit(DataWritten());
        CloseIfOpen();
    }
}

void SerialPortWriter::HandleTimeout()
{
    qDebug() << "SerialPortWriter::HandleTimeout";
    CloseIfOpen();
    qWarning() << QObject::tr("Operation timed out for port %1, error: %2").arg(serial_port_.portName()).arg(serial_port_.errorString());
    emit(WriteError(serial_port_.errorString()));
}

void SerialPortWriter::HandleError(QSerialPort::SerialPortError serialPortError)
{
    qDebug() << "SerialPortWriter::HandleError";
    CloseIfOpen();
    if (serialPortError == QSerialPort::WriteError) {
        qWarning() << QObject::tr("An I/O error occurred while writing the data to port %1, error: %2").arg(serial_port_.portName()).arg(serial_port_.errorString());
        emit(WriteError(serial_port_.errorString()));
    }
}

void SerialPortWriter::CloseIfOpen() {
    if (serial_port_.isOpen()) {
        serial_port_.close();
    }
}

void SerialPortWriter::Write(const QByteArray &data)
{
    qDebug() << "SerialPortWriter::Write: " << data;
    bytes_written_ = 0;
    data_ = data;
    qint64 bytes_written = serial_port_.write(data);

    if (bytes_written == -1) {
        CloseIfOpen();
        qWarning() << QObject::tr("Failed to write the data to port %1, error: %2").arg(serial_port_.portName()).arg(serial_port_.errorString());
        emit(WriteError(serial_port_.errorString()));
    } else if (bytes_written != data_.size()) {
        serial_port_.close();
        qWarning() << QObject::tr("Failed to write all the data to port %1, error: %2").arg(serial_port_.portName()).arg(serial_port_.errorString());
        emit(WriteError(serial_port_.errorString()));
    } else {
        timer_.start(timeout_ms_);
    }
}
