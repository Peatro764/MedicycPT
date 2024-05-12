#include "SocketClient.h"

#include <QDebug>
#include <QDateTime>

#include <Util.h>

SocketClient::SocketClient(QString ip, int port,
                           int connection_timeout, int read_timeout, int write_timeout,
                           QByteArray cmd_sep)
    : ip_(ip),
      port_(port),
      read_timeout_ms_(read_timeout),
      write_timeout_ms_(write_timeout),
      connection_timeout_ms_(connection_timeout),
      bytes_remaining_to_write_(0),
      read_data_buffer_(""),
      read_timer_(this),
      write_timer_(this),
      connection_timer_(this),
      cmd_sep_(cmd_sep)
{            
    read_timer_.setSingleShot(true);
    write_timer_.setSingleShot(true);
    connection_timer_.setSingleShot(true);

    QObject::connect(&socket_, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
                     this, &SocketClient::HandleSocketError);

    QObject::connect(&read_timer_, &QTimer::timeout, this, &SocketClient::HandleReadTimeout);
    QObject::connect(&write_timer_, &QTimer::timeout, this, &SocketClient::HandleWriteTimeout);
    QObject::connect(&write_timer_, &QTimer::timeout, this, [&]() { bytes_remaining_to_write_ = 0; });
    QObject::connect(&connection_timer_, &QTimer::timeout, this, &SocketClient::HandleConnectionTimeout);

    QObject::connect(&socket_, &QTcpSocket::connected, this, &SocketClient::ConnectionEstablished);
    QObject::connect(&socket_, &QTcpSocket::connected, this, [&]() { connection_timer_.stop(); });

    QObject::connect(&socket_, &QTcpSocket::stateChanged, this, &SocketClient::StateChanged);
    QObject::connect(&socket_, &QIODevice::readyRead, this, &SocketClient::ReadAvailableData);
    QObject::connect(&socket_, &QTcpSocket::bytesWritten, this, &SocketClient::BytesWritten);
}

SocketClient::~SocketClient() {
    socket_.abort();
}

void SocketClient::Connect() {
    if (socket_.state() == QAbstractSocket::UnconnectedState) {
        QString msg("Connecting to " + ip_ + ":" + QString::number(port_));
        qDebug() << QDateTime::currentDateTime() << " " << msg;
        socket_.connectToHost(ip_, static_cast<quint16>(port_), QIODevice::ReadWrite, QAbstractSocket::AnyIPProtocol);
        connection_timer_.start(connection_timeout_ms_);
    } else {
        qDebug() << QDateTime::currentDateTime() << " SocketClient::Connect Already connecting/connected";
    }
}

void SocketClient::Disconnect() {
    socket_.abort();
}

void SocketClient::Write(const QByteArray& data) {
    const qint64 bytes_written = socket_.write(data);
    bytes_remaining_to_write_ += bytes_written;
    if (bytes_written == data.size()) {
        write_timer_.start(write_timeout_ms_);
    } else {
        qWarning() << QDateTime::currentDateTime() << QObject::tr("SocketWriter::Write Failed to write the data to port %1, error: %2").arg(ip_).arg(socket_.errorString());
        emit(WriteError(socket_.errorString()));
    }
}

void SocketClient::Write(const QString& data) {
    Write(data.toUtf8());
}

void SocketClient::Flush() {
    qDebug() << "SocketClient::Flush";
    socket_.flush();
    read_data_buffer_.clear();
    bytes_remaining_to_write_ = 0;
    read_timer_.stop();
    write_timer_.stop();
}

void SocketClient::BytesWritten(qint64 bytes) {
    bytes_remaining_to_write_ -= bytes;
    if (bytes_remaining_to_write_ == 0) {
        emit(DataWritten());
        write_timer_.stop();
    } else {
        write_timer_.start(write_timeout_ms_);
    }

}

void SocketClient::HandleConnectionTimeout() {
    qDebug() << QDateTime::currentDateTime() << " SocketClient::HandleConnectionTimeout";
    socket_.abort();
    emit(PortError("Échec de connexion"));
}

void SocketClient::HandleWriteTimeout() {
    qDebug() << QDateTime::currentDateTime() << " SocketClient::HandleWriteTimeout Write timeout";
    emit(WriteError("Write timout"));
}

void SocketClient::HandleSocketError(QAbstractSocket::SocketError error) {
    (void)error;
    qDebug() << QDateTime::currentDateTime() << " SocketClient::HandleSocketError " << socket_.errorString();
    socket_.abort();
    connection_timer_.stop();
    emit(PortError(socket_.errorString()));
}

void SocketClient::HandleReadTimeout() {
    qDebug() << QDateTime::currentDateTime().toString() << " SocketClient::HandleReadTimeout";
    emit(ReadError("Timeout reading command: " + read_data_buffer_));
    read_data_buffer_.clear();
}

void SocketClient::ReadAvailableData() {
    auto bytes_available(socket_.bytesAvailable());
    read_data_buffer_.append(socket_.read(bytes_available));

    std::vector<QByteArray> messages = util::Split(read_data_buffer_, cmd_sep_);
    read_data_buffer_.clear();
    if (messages.empty()) {
        qWarning() << "SocketClient::ReadAvailableData Messages empty..";
    } else {
        for (int idx = 0; idx < (static_cast<int>(messages.size()) - 1); ++idx) {
            emit (DataRead(messages.at(idx)));
        }
        const QByteArray rest = messages.back();
        if (rest.endsWith(cmd_sep_)) {
            emit (DataRead(rest));
            read_timer_.stop();
        } else {
            read_data_buffer_ = rest;
            read_timer_.start(read_timeout_ms_);
        }
    }
}

void SocketClient::StateChanged(QAbstractSocket::SocketState state) {
    qDebug() << QDateTime::currentDateTime() << " SocketState " << state;
    if (state == QAbstractSocket::UnconnectedState) {
        socket_.abort();
        emit (Disconnected());
    }
}

