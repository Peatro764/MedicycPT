#include "SocketServer.h"

#include <QDebug>
#include <QTcpSocket>

SocketServer::SocketServer(int port)
    : port_(port)
{
    if (tcp_server_.listen(QHostAddress::Any, port_)) {
        qDebug() << "SocketServer::SocketServer Server started successfully, listening on port " << port_;
    } else {
        qWarning() << "SocketServer::SocketServer Server could not start up";
        return;
    }
    QObject::connect(&tcp_server_, &QTcpServer::newConnection, this, &SocketServer::AddConnection);
}

SocketServer::~SocketServer() {
    for (QTcpSocket* client : client_connections_) {
        client->disconnectFromHost();
    }
}

void SocketServer::AddConnection() {
    QTcpSocket *client_connection = tcp_server_.nextPendingConnection();
    qDebug() << "SocketServer::AddConnection New client connected: " << client_connection->peerAddress();
    client_connections_.push_back(client_connection);
}

void SocketServer::WriteClients(QString data) {
    for (QTcpSocket* client : client_connections_) {
        qDebug() << "SocketServer::WriteClients Writing data " << data << " to client " << client->peerAddress();
        client->write(data.toStdString().c_str(), data.size());
    }
}


