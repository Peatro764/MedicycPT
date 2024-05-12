#include "SocketReader.h"

#include <QDebug>

SocketReader::SocketReader(QSettings& settings)
    : bytes_read_(0)
    , read_timeout_ms_(10000)
    , connection_timeout_ms_(3000)

{
    connection_timer_.setSingleShot(true);
    read_timer_.setSingleShot(true);

    connection_timeout_ms_ = settings.value("tcpip_comm/connection_timeout", "3000").toInt();
    read_timeout_ms_ = settings.value("tcpip_comm/read_timeout", "300000").toInt();
    ip_ = settings.value("tcpip_comm/ip", "localhost").toString();
    port_ = settings.value("tcpip_comm/port", "3000").toString();

    QObject::connect(&socket_, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
                     this, &SocketReader::HandleError);
    QObject::connect(&read_timer_, &QTimer::timeout, this, &SocketReader::HandleReadTimeout);
    QObject::connect(&connection_timer_, &QTimer::timeout, this, &SocketReader::HandleConnectionTimeout);
    QObject::connect(&socket_, SIGNAL(connected()), this, SLOT(ConnectionEstablished()));
}


SocketReader::~SocketReader()
{
    Cleanup();
}

void SocketReader::Abort() {
    Cleanup();
}

void SocketReader::Cleanup() {
    qDebug() << "SocketReader::Cleanup";
    connection_timer_.stop();
    read_timer_.stop();
    QObject::disconnect(&socket_, &QIODevice::readyRead, this, &SocketReader::ReadAvailableData);
    socket_.abort();
}

void SocketReader::ConnectionEstablished() {
    qDebug() << "SocketReader::ConnectionEstablished";
    connection_timer_.stop();
}

void SocketReader::HandleConnectionTimeout() {
    Cleanup();
    qWarning() << QObject::tr("SocketReader::HandleConnectionTimeout Connection timed out for host %1, error: %2").arg(ip_).arg(socket_.errorString());
    emit(ReadError(socket_.errorString()));
}

void SocketReader::HandleReadTimeout()
{
    Cleanup();
    qWarning() << QObject::tr("SocketReader::HandleReadTimeout Operation timed out for host %1, error: %2").arg(ip_).arg(socket_.errorString());
    emit(ReadError(QString("Read timeout")));
}

void SocketReader::HandleError(QAbstractSocket::SocketError error)
{
    (void)error;
    Cleanup();
    qWarning() << QObject::tr("SocketReader::HandleError An I/O error occurred while reading from host %1, error: %2").arg(ip_).arg(socket_.errorString());
    emit(ReadError(socket_.errorString()));
}

void SocketReader::Read() {
    data_.clear();
    Open();
    read_timer_.start(read_timeout_ms_);
    QObject::connect(&socket_, &QIODevice::readyRead, this, &SocketReader::ReadAvailableData);
}

void SocketReader::ReadAvailableData() {
    auto bytes_available(socket_.bytesAvailable());
    QByteArray data(socket_.read(bytes_available));
    qDebug() << "SocketWriter::ReadAvailableData Read data: " << data;
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

void SocketReader::Open() {
    if (socket_.state() == QAbstractSocket::UnconnectedState) {
        qDebug() << "SocketReader::Open Connecting...";
        socket_.connectToHost(ip_, port_.toInt(), QIODevice::ReadOnly, QAbstractSocket::AnyIPProtocol);
        connection_timer_.start(connection_timeout_ms_);
    } else {
        qDebug() << "SocketReader::Open Already connected";
    }
}
