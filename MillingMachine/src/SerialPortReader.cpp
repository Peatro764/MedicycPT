#include "SerialPortReader.h"

#include <QDebug>

SerialPortReader::SerialPortReader(QSettings& settings)
    : bytes_read_(0)
    , timeout_ms_(10000)
{
    timer_.setSingleShot(true);
    timeout_ms_ = settings.value("serial_comm/timeout", "3000").toInt();
    port_ = settings.value("serial_comm/port", "unknown").toString();
    baud_rate_ = settings.value("serial_comm/baudrate", "9600").toInt();
    data_bits_ = static_cast<QSerialPort::DataBits>(settings.value("serial_comm/databits", "7").toInt());
    parity_ = static_cast<QSerialPort::Parity>(settings.value("serial_comm/parity","0").toInt());
    stop_bits_ = static_cast<QSerialPort::StopBits>(settings.value("serial_comm/stopbits", "1").toInt());
    flow_control_ = static_cast<QSerialPort::FlowControl>(settings.value("serial_comm/flowcontrol", "0").toInt());

    QObject::connect(&serial_port_, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
            this, &SerialPortReader::HandleError);
    QObject::connect(&timer_, &QTimer::timeout, this, &SerialPortReader::HandleTimeout);
}


SerialPortReader::~SerialPortReader()
{
    Cleanup();
}

void SerialPortReader::Abort() {
    Cleanup();
}

void SerialPortReader::Cleanup() {
    qDebug() << "SerialPortReader::Cleanup";
    timer_.stop();
    QObject::disconnect(&serial_port_, &QIODevice::readyRead, this, &SerialPortReader::ReadAvailableData);
    serial_port_.close();
}

void SerialPortReader::HandleTimeout() {
    Cleanup();
    qWarning() << QObject::tr("SerialPortReader::HandleTimeout Connection timed out for port %1, error: %2").arg(serial_port_.portName()).arg(serial_port_.errorString());
    emit(ReadError("Read timeout"));
}

void SerialPortReader::HandleError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError == QSerialPort::WriteError) {
        qWarning() << QObject::tr("An error occurred while reading from port %1, error: %2").arg(serial_port_.portName()).arg(serial_port_.errorString());
        emit(ReadError(serial_port_.errorString()));
    }
}


void SerialPortReader::Read() {
    if (!Open()) {
        return;
    }
    data_.clear();
    timer_.start(timeout_ms_);
    QObject::connect(&serial_port_, &QIODevice::readyRead, this, &SerialPortReader::ReadAvailableData);
}

void SerialPortReader::ReadAvailableData() {
    QByteArray data(serial_port_.readAll());
    qDebug() << "SerialPortReader::ReadAvailableData Read data: " << data;
    data_.append(data);
    if (data_.contains("END PGM")) {
        int pgm_start_index(data_.indexOf("0 BEGIN PGM"));
        if (pgm_start_index != -1) {
            data_ = data_.right(data_.size() - pgm_start_index);
        }
        Cleanup();
        emit(ProgramRead(data_));
    }
}

bool SerialPortReader::Open() {
    serial_port_.setPortName(port_);
    qDebug() << "SerialPortReader Opening port " << port_;
    if (serial_port_.isOpen()) {
        qDebug() << "SerialPortReader Port already opened";
        serial_port_.close();
    }

    if (!serial_port_.open(QIODevice::ReadOnly)) {
        emit(ReadError(QString("Could not open serial port: ") + serial_port_.errorString()));
        qWarning() << QString("SerialPortReader::Open Could not open serial port: ") + serial_port_.errorString();
        serial_port_.close();
        return false;
    }

    if (!serial_port_.setBaudRate(baud_rate_)) {
        qDebug() << "SerialPortReader::Open SetBaudRate failed";
        return false;
    } else {
        qDebug() << "SerialPortReader::Open BaudRate = " << baud_rate_;
    }
    if (!serial_port_.setDataBits(data_bits_)) {
        qDebug() << "SerialPortReader::Open SetDataBits failed";
        return false;
    } else {
        qDebug() << "SerialPortReader::Open DataBits = " << data_bits_;
    }
    if (!serial_port_.setParity(parity_)) {
        qDebug() << "SerialPortReader::Open SetParity failed";
        return false;
    } else {
        qDebug() << "SerialPortReader::Open Parity = " << parity_;
    }
    if (!serial_port_.setStopBits(stop_bits_)) {
        qDebug() << "SerialPortReader::Open SetStopBits failed";
        return false;
    } else {
        qDebug() << "SerialPortReader::Open StopBits = " << stop_bits_;
    }
    if (!serial_port_.setFlowControl(flow_control_)) {
        qDebug() << "SerialPortReader::Open SetFlowControl failed";
        return false;
    } else {
        qDebug() << "SerialPortReader::Open FlowControl = " << flow_control_;
    }
    return true;
}
