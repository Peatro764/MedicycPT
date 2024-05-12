#include "SocketWriter.h"

#include <QDebug>

SocketWriter::SocketWriter(QSettings& settings)
    : bytes_written_(0)
    , write_timeout_ms_(1000)
    , connection_timeout_ms_(3000)

{
    connection_timer_.setSingleShot(true);
    close_connection_timer_.setSingleShot(true);
    write_timer_.setSingleShot(true);

    connection_timeout_ms_ = settings.value("tcpip_comm/connection_timeout", "3000").toInt();
    write_timeout_ms_ = settings.value("tcpip_comm/write_timeout", "1000").toInt();
    ip_ = settings.value("tcpip_comm/ip", "localhost").toString();
    port_ = settings.value("tcpip_comm/port", "3000").toString();

    QObject::connect(&socket_, &QTcpSocket::bytesWritten, this, &SocketWriter::HandleBytesWritten);
    QObject::connect(&socket_, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
                     this, &SocketWriter::HandleError);
    QObject::connect(&write_timer_, &QTimer::timeout, this, &SocketWriter::HandleWriteTimeout);
    QObject::connect(&connection_timer_, &QTimer::timeout, this, &SocketWriter::HandleConnectionTimeout);
    QObject::connect(&socket_, SIGNAL(connected()), this, SLOT(ConnectionEstablished()));
    QObject::connect(&close_connection_timer_, &QTimer::timeout, this, &SocketWriter::CloseConnection);
}


SocketWriter::~SocketWriter()
{
    Cleanup();
    CloseConnection();
}

void SocketWriter::Abort() {
    Cleanup();
    CloseConnection();
}

void SocketWriter::Cleanup() {
    qDebug() << "SocketWriter::Cleanup";
    bytes_written_ = 0;
    connection_timer_.stop();
    write_timer_.stop();
    QObject::disconnect(&socket_, &QTcpSocket::connected, this, &SocketWriter::WriteData);
//    socket_.disconnectFromHost();
//    socket_.abort();
}

void SocketWriter::CloseConnection() {
    qDebug() << "SocketWriter::CloseConnection";
    socket_.disconnectFromHost();
}

void SocketWriter::ConnectionEstablished() {
    connection_timer_.stop();
    qDebug() << "SocketWriter::ConnectionEstablished";
}

void SocketWriter::HandleBytesWritten(qint64 bytes) {
    bytes_written_ += bytes;
    qDebug() << "bytes " << QString::number(bytes_written_) << " (" << QString::number(data_.size()) + ")";
    if (bytes_written_ == data_.size()) {
        qDebug() << QObject::tr("SocketWriter::HandleBytesWritten Data successfully sent to host %1").arg(ip_);
        Cleanup();
        close_connection_timer_.start(60000);
        emit(DataWritten());
    } else {
        qDebug() << "SocketWriter::HandleBytesWritten Partial write" << bytes;
    }
}

void SocketWriter::HandleConnectionTimeout() {
    Cleanup();
    CloseConnection();
    qWarning() << QObject::tr("SocketWriter::HandleConnectionTimeout Connection timed out for host %1, error: %2").arg(ip_).arg(socket_.errorString());
    emit(WriteError(socket_.errorString()));
}

void SocketWriter::HandleWriteTimeout()
{
    Cleanup();
    CloseConnection();
    qWarning() << QObject::tr("SocketWriter::HandleWriteTimeout Operation timed out for host %1, error: %2").arg(ip_).arg(socket_.errorString());
    emit(WriteError(socket_.errorString()));
}

void SocketWriter::HandleError(QAbstractSocket::SocketError error)
{
    (void)error;
    Cleanup();
    CloseConnection();
    qWarning() << QObject::tr("SocketWriter::HandleError An I/O error occurred while writing the data to host %1, error: %2").arg(ip_).arg(socket_.errorString());
    emit(WriteError(socket_.errorString()));
}

void SocketWriter::Write(const QByteArray &data) {
    qDebug() << "SocketWriter::Write " << data;
    bytes_written_ = 0;
    data_ = data;

    Open();

    if (socket_.state() != QAbstractSocket::ConnectedState) {
        qDebug() << "SocketWriter::Write Not yet connected, waiting a bit";
        QObject::connect(&socket_, &QTcpSocket::connected, this, &SocketWriter::WriteData);
    } else {
        qDebug() << "SocketWriter::Write Connected, writing at once";
        WriteData();
    }
}

void SocketWriter::Open() {
    if (socket_.state() == QAbstractSocket::UnconnectedState) {
        qDebug() << "SocketWriter::Open Connection...";
        socket_.connectToHost(ip_, port_.toInt(), QIODevice::ReadWrite, QAbstractSocket::AnyIPProtocol);
        connection_timer_.start(connection_timeout_ms_);
    } else {
        qDebug() << "SocketWriter::Open Already connected";
    }
}

void SocketWriter::WriteData()
{
    qDebug() << "SocketWriter::WriteData";
    QObject::disconnect(&socket_, &QTcpSocket::connected, this, &SocketWriter::WriteData);

    qint64 bytes_written = socket_.write(data_);

    if (bytes_written == -1) {
        qWarning() << QObject::tr("SocketWriter::Write Failed to write the data to port %1, error: %2").arg(ip_).arg(socket_.errorString());
        Cleanup();
        CloseConnection();
        emit(WriteError(socket_.errorString()));
    } else if (bytes_written != data_.size()) {
        Cleanup();
        CloseConnection();
        qWarning() << QObject::tr("SocketWriter::Write Failed to write all the data to port %1, error: %2").arg(ip_).arg(socket_.errorString());
        emit(WriteError(socket_.errorString()));
    } else {
        qDebug() << "SocketWriter::WriteData In progress...";
        write_timer_.start(write_timeout_ms_);
    }
}
