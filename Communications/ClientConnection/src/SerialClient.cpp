#include "SerialClient.h"

#include <QDebug>
#include <QDateTime>

#include "Util.h"

SerialClient::SerialClient(QString port, QSerialPort::BaudRate baud_rate, QSerialPort::DataBits data_bits,
                          QSerialPort::Parity parity, QSerialPort::StopBits stop_bits, QSerialPort::FlowControl flow_control,
                          int connection_timeout_ms, int read_timeout_ms, int write_timeout_ms,
                          QByteArray separator)
    : port_(port), baud_rate_(baud_rate), data_bits_(data_bits), parity_(parity), stop_bits_(stop_bits),
      flow_control_(flow_control), connection_timeout_ms_(connection_timeout_ms), read_timeout_ms_(read_timeout_ms),
      write_timeout_ms_(write_timeout_ms), bytes_remaining_to_write_(0), separator_(separator) {

    read_timer_.setSingleShot(true);
    write_timer_.setSingleShot(true);
    connection_timer_.setSingleShot(true);

    QObject::connect(&serial_port_, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
            this, &SerialClient::HandleError);

    QObject::connect(&read_timer_, &QTimer::timeout, this, &SerialClient::HandleReadTimeout);
    QObject::connect(&write_timer_, &QTimer::timeout, this, &SerialClient::HandleWriteTimeout);
    QObject::connect(&connection_timer_, &QTimer::timeout, this, &SerialClient::Connect);

    QObject::connect(&serial_port_, &QIODevice::readyRead, this, &SerialClient::ReadAvailableData);
    QObject::connect(&serial_port_, &QSerialPort::bytesWritten, this, &SerialClient::BytesWritten);
}

SerialClient::~SerialClient() {}

void SerialClient::Connect() {
    qDebug() << "SerialClient Opening port " << port_;
    serial_port_.setPortName(port_);
    if (serial_port_.isOpen()) {
        serial_port_.close();
    }

    if (!serial_port_.open(QIODevice::ReadWrite)) {
        qWarning() << QString("SerialClient::Connect Could not open serial port: ") + serial_port_.errorString();
        serial_port_.close();
        return;
    }
    emit(ConnectionEstablished());

    if (!serial_port_.setBaudRate(baud_rate_)) {
        qWarning() << "SerialClient::Connect Serial port SetBaudRate failed";
        return;
    }
    if (!serial_port_.setDataBits(data_bits_)) {
        qWarning() << "SerialClient::Connect Serial port SetDataBits failed";
        return;
    }
    if (!serial_port_.setParity(parity_)) {
        qWarning() << "SerialClient::Connect Serial port SetParity failed";
        return;
    }
    if (!serial_port_.setStopBits(stop_bits_)) {
        qWarning() << "SerialClient::Connect Serial port SetStopBits failed";
        return;
    }
    if (!serial_port_.setFlowControl(flow_control_)) {
        qWarning() << "SerialClient::Connect Serial port SetFlowControl failed";
        return;
    }

    qWarning() << "SerialClient::Connect Serial port opened";
    return;
}

void SerialClient::Write(const QByteArray& data) {
    const qint64 bytes_written = serial_port_.write(data);
    bytes_remaining_to_write_ += bytes_written;
    if (bytes_written == data.size()) {
        write_timer_.start(write_timeout_ms_);
    } else {
        qWarning() << QDateTime::currentDateTimeUtc().toString() <<
                      "SerialClient::Write Failed to write the data to port " << serial_port_.errorString();
        emit(WriteError(serial_port_.errorString()));
    }
}

void SerialClient::Write(const QString& data) {
    Write(data.toUtf8());
}


void SerialClient::Flush() {
    qDebug() << "SerialClient::Flush";
    serial_port_.flush();
    read_data_buffer_.clear();
    bytes_remaining_to_write_ = 0;
    read_timer_.stop();
    write_timer_.stop();
}

void SerialClient::BytesWritten(qint64 bytes) {
    bytes_remaining_to_write_ -= bytes;
    if (bytes_remaining_to_write_ == 0) {
        qWarning() << "SerialClient::Connect Data successfully sent to port " << serial_port_.portName();
        write_timer_.stop();
    } else {
        write_timer_.start(write_timeout_ms_);
    }
}

void SerialClient::HandleWriteTimeout() {
    write_timer_.stop();
    emit(WriteError("Write timout"));
}

void SerialClient::HandleError(QSerialPort::SerialPortError error) {
    (void)error; // to silence warning
    if (serial_port_.errorString() != QString("No error")) {
        emit(PortError(serial_port_.errorString()));
        CloseIfOpen();
        connection_timer_.start(connection_timeout_ms_);
    }
}

void SerialClient::HandleReadTimeout() {
    emit(ReadError("Timeout reading command: " + read_data_buffer_));
    read_data_buffer_.clear();
}

void SerialClient::ReadAvailableData() {
    auto bytes_available(serial_port_.bytesAvailable());
    read_data_buffer_.append(serial_port_.read(bytes_available));

    std::vector<QByteArray> messages = util::Split(read_data_buffer_, separator_);
    read_data_buffer_.clear();
    if (messages.empty()) {
        qWarning() << "SocketClient::ReadAvailableData Messages empty..";
    } else {
        for (int idx = 0; idx < (static_cast<int>(messages.size()) - 1); ++idx) {
            emit (DataRead(messages.at(idx)));
        }
        const QByteArray rest = messages.back();
        if (rest.endsWith(separator_)) {
            emit (DataRead(rest));
            read_timer_.stop();
        } else {
            read_data_buffer_ = rest;
            read_timer_.start(read_timeout_ms_);
        }
    }
}

void SerialClient::CloseIfOpen() {
    if (serial_port_.isOpen()) {
        serial_port_.close();
    }
}



