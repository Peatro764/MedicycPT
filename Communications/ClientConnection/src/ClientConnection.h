#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include <QObject>

class ClientConnection
{

public:
    virtual ~ClientConnection() {}
    virtual bool Connected() const = 0;

public slots:
    virtual void Connect() = 0;
    virtual void Disconnect() = 0;
    virtual void Write(const QString& data) = 0;
    virtual void Write(const QByteArray& data) = 0;
    virtual void Flush() = 0;

signals:
    virtual void DataWritten() = 0;
    virtual void DataRead(const QByteArray& data) = 0;
    virtual void ReadError(const QString& error) = 0;
    virtual void WriteError(const QString& error) = 0;
    virtual void PortError(const QString& error) = 0;
    virtual void ConnectionEstablished() = 0;
    virtual void Disconnected() = 0;
};

Q_DECLARE_INTERFACE(ClientConnection, "ClientConnection")

#endif
