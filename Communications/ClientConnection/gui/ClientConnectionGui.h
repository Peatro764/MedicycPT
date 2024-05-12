#ifndef CLIENTCONNECTIONGUI_H
#define CLIENTCONNECTIONGUI_H

#include <QWidget>
#include <QTimer>

#include "SerialClient.h"
#include "SocketClient.h"

namespace Ui {
class ClientConnectionGui;
}

class ClientConnectionGui : public QWidget
{
    Q_OBJECT

public:
    explicit ClientConnectionGui(QWidget *parent = 0);
    ~ClientConnectionGui();

public slots:

private slots:
    void ConnectSerialPort();
    void WriteSerialData();

    void ConnectSocket();
    void WriteSocketData();
    void WriteSerial();
    void WriteSocket();
    void AppendText(QByteArray data);

private:
    void ConnectSerialSignals();
    void GetAvailableSerialPorts();
    void FillSerialPortOptions();
    QByteArray GetSerialLinebreak();

    void ConnectSocketSignals();
    QByteArray GetSocketLinebreak();

    Ui::ClientConnectionGui *ui_;
    SerialClient* serial_ = nullptr;
    SocketClient* socket_ = nullptr;

    enum LINEBREAK {
        CRLF,
        CR,
        LF,
        NONE
    };

    QByteArray serial_linebreak_ = "\r\n";
    QByteArray socket_linebreak_ = "MTD";

    QTimer timer_;
};


#endif
