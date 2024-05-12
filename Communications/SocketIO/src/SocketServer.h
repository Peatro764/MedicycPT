#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <QTcpServer>

class SocketServer : public QObject
{
    Q_OBJECT

public:
    explicit SocketServer(int port);
    ~SocketServer();

public slots:
    void AddConnection();
    void WriteClients(QString data);

private slots:

signals:

private:
    int port_;
    QTcpServer tcp_server_;
    QList<QTcpSocket*> client_connections_;
};

#endif
