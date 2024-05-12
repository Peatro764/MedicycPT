#include "SocketClient.h"

#include <QDebug>
#include <QDateTime>

SocketClient::SocketClient(QString ip, int port,
                           int connection_timeout, int reconnection_timeout,
                           int read_timeout, int write_timeout,
                           QString cmd_sep,
                           bool reconnect_on_timeout = false)
    : ip_(ip),
      port_(port),
      read_timeout_ms_(read_timeout),
      write_timeout_ms_(write_timeout),
      connection_timeout_ms_(connection_timeout),
      reconnection_timeout_ms_(reconnection_timeout),
      bytes_remaining_to_write_(0),
      read_data_buffer_(""),
      cmd_sep_(cmd_sep),
      reconnect_on_timeout_(reconnect_on_timeout)
{
    read_timer_.setSingleShot(true);
    write_timer_.setSingleShot(true);
    connection_timer_.setSingleShot(true);
    reconnection_timer_.setSingleShot(true);

    QObject::connect(&socket_, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
                     this, &SocketClient::HandleSocketError);

    QObject::connect(&read_timer_, &QTimer::timeout, this, &SocketClient::HandleReadTimeout);
    QObject::connect(&write_timer_, &QTimer::timeout, this, &SocketClient::HandleWriteTimeout);
    QObject::connect(&write_timer_, &QTimer::timeout, this, [&]() { bytes_remaining_to_write_ = 0; });
    QObject::connect(&connection_timer_, &QTimer::timeout, this, &SocketClient::HandleConnectionTimeout);
    QObject::connect(&reconnection_timer_, &QTimer::timeout, this, &SocketClient::Connect);

    QObject::connect(&socket_, &QTcpSocket::connected, this, &SocketClient::ConnectionEstablished);
    QObject::connect(&socket_, &QTcpSocket::connected, this, [&]() { connection_timer_.stop(); });

    QObject::connect(&socket_, &QTcpSocket::stateChanged, this, &SocketClient::StateChanged);
    QObject::connect(&socket_, &QIODevice::readyRead, this, &SocketClient::ReadAvailableData);
    QObject::connect(&socket_, &QTcpSocket::bytesWritten, this, &SocketClient::BytesWritten);
    QObject::connect(&socket_, &QTcpSocket::disconnected, this, &SocketClient::Disconnected);
}

SocketClient::~SocketClient() {}

void SocketClient::StateChanged(QAbstractSocket::SocketState state) {
    qDebug() << QDateTime::currentDateTimeUtc().toString() << " SocketClient::StateChanged " << state;
}

void SocketClient::Connect() {
    qDebug() << QDateTime::currentDateTimeUtc().toString() << " SocketClient::Connect " << socket_.state();
    if (socket_.state() == QAbstractSocket::UnconnectedState) {
        QString msg("Connecting to " + ip_ + ":" + QString::number(port_));
        qDebug() << QDateTime::currentDateTimeUtc().toString() << " SocketClient::Connect " << msg;
        socket_.connectToHost(ip_, static_cast<quint16>(port_), QIODevice::ReadWrite, QAbstractSocket::AnyIPProtocol);
        connection_timer_.start(connection_timeout_ms_);
    } else {
        qDebug() << QDateTime::currentDateTimeUtc().toString() << " SocketClient::Connect Already connecting/connected";
    }
}

void SocketClient::Write(const QByteArray& data) {
    const qint64 bytes_written = socket_.write(data);
    qDebug() << "SocketClient::Write bytes written = " << bytes_written;
    bytes_remaining_to_write_ += bytes_written;
    if (bytes_written == data.size()) {
        write_timer_.start(write_timeout_ms_);
    } else {
        qWarning() << QDateTime::currentDateTimeUtc().toString() << QObject::tr("SocketWriter::Write Failed to write the data to port %1, error: %2").arg(ip_).arg(socket_.errorString());
        emit(WriteError(socket_.errorString()));
    }
}

void SocketClient::Write(const QString& data) {
    Write(data.toUtf8());
}

void SocketClient::BytesWritten(qint64 bytes) {
    //qDebug() << QDateTime::currentDateTimeUtc().toString() << " SocketClient::BytesWritten " << QString::number(bytes);
    bytes_remaining_to_write_ -= bytes;
    if (bytes_remaining_to_write_ == 0) {
        emit(DataWritten());
        write_timer_.stop();
    } else {
        write_timer_.start(write_timeout_ms_);
    }
}

void SocketClient::HandleConnectionTimeout() {
    qDebug() << QDateTime::currentDateTimeUtc().toString() << " SocketClient::HandleConnectionTimeout";
    emit(SocketError("Connection timout"));
    if (reconnect_on_timeout_) reconnection_timer_.start(reconnection_timeout_ms_);
}

void SocketClient::HandleWriteTimeout() {
    qDebug() << QDateTime::currentDateTimeUtc().toString() << " SocketClient::HandleWriteTimeout Write timeout";
    write_timer_.stop();
    emit(WriteError("Write timout"));
}

void SocketClient::HandleSocketError(QAbstractSocket::SocketError error) {
    (void)error;
    qDebug() << QDateTime::currentDateTimeUtc().toString() << " SocketClient::HandleSocketError " << socket_.errorString();
    emit(SocketError(socket_.errorString()));
    socket_.close();
    if (reconnect_on_timeout_) reconnection_timer_.start(reconnection_timeout_ms_);
}

void SocketClient::HandleReadTimeout() {
    qDebug() << QDateTime::currentDateTimeUtc().toString() << " SocketClient::HandleReadTimeout";
    emit(ReadError("Timeout reading command: " + read_data_buffer_));
    read_data_buffer_.clear();
}

void SocketClient::ReadAvailableData() {
    auto bytes_available(socket_.bytesAvailable());
    QByteArray data(socket_.read(bytes_available));

    if (cmd_sep_.isEmpty()) {
        emit(DataRead(data));
    } else {
        read_data_buffer_.append(data);
        QStringList words(read_data_buffer_.split(cmd_sep_, QString::SkipEmptyParts));
        for (int idx = 0; idx < words.size(); ++idx) {
            auto w(words.at(idx));
            if (idx < (words.size() - 1) || read_data_buffer_.endsWith(cmd_sep_)) {
                emit(DataRead(w + cmd_sep_));
                read_data_buffer_ = read_data_buffer_.right(read_data_buffer_.size() - (w.size() + cmd_sep_.size()));
                read_timer_.stop();
            } else {
                read_timer_.start(read_timeout_ms_);
            }
        }
    }
}
