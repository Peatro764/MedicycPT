#include "ClientConnectionGui.h"
#include "ui_ClientConnectionGui.h"

#include <unistd.h>
#include "QSerialPortInfo"

ClientConnectionGui::ClientConnectionGui(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::ClientConnectionGui)
{
    ui_->setupUi(this);

    GetAvailableSerialPorts();
    FillSerialPortOptions();

    QObject::connect(ui_->pushButton_connectSerialClient, SIGNAL(clicked()), this, SLOT(ConnectSerialPort()));
    QObject::connect(ui_->lineEdit_writeDataSerialClient, SIGNAL(returnPressed()) , this, SLOT(WriteSerialData()));
    QObject::connect(ui_->pushButton_connectSocketClient, SIGNAL(clicked()), this, SLOT(ConnectSocket()));
    QObject::connect(ui_->lineEdit_writeDataSocketClient, SIGNAL(returnPressed()) , this, SLOT(WriteSocketData()));
}

ClientConnectionGui::~ClientConnectionGui()
{
}

void ClientConnectionGui::ConnectSerialSignals() {
    QObject::connect(serial_, SIGNAL(PortError(QString)), ui_->plainTextEdit_serialClient, SLOT(appendPlainText(QString)));
    QObject::connect(serial_, SIGNAL(WriteError(QString)), ui_->plainTextEdit_serialClient, SLOT(appendPlainText(QString)));
    QObject::connect(serial_, SIGNAL(ReadError(QString)), ui_->plainTextEdit_serialClient, SLOT(appendPlainText(QString)));
    QObject::connect(serial_, SIGNAL(DataRead(QString)), ui_->plainTextEdit_serialClient, SLOT(appendPlainText(QString)));
    QObject::connect(serial_, SIGNAL(DataRead(QByteArray)), this, SLOT(AppendText(QByteArray)));
    QObject::connect(ui_->pushButton_disconnectSerialClient, SIGNAL(clicked(bool)) , serial_, SLOT(Disconnect()));
}

void ClientConnectionGui::ConnectSocketSignals() {
    QObject::connect(socket_, SIGNAL(PortError(QString)), ui_->plainTextEdit_serialClient, SLOT(appendPlainText(QString)));
    QObject::connect(socket_, SIGNAL(WriteError(QString)), ui_->plainTextEdit_serialClient, SLOT(appendPlainText(QString)));
    QObject::connect(socket_, SIGNAL(ReadError(QString)), ui_->plainTextEdit_serialClient, SLOT(appendPlainText(QString)));
    QObject::connect(socket_, SIGNAL(DataRead(QString)), ui_->plainTextEdit_serialClient, SLOT(appendPlainText(QString)));
    QObject::connect(socket_, SIGNAL(DataRead(QByteArray)), this, SLOT(AppendText(QByteArray)));
    QObject::connect(ui_->pushButton_disconnectSocketClient, SIGNAL(clicked(bool)) , socket_, SLOT(Disconnect()));
    QObject::connect(socket_, &SocketClient::Disconnected, this, [=](){ qDebug() << "JDJD"; ui_->radioButton_socket_connected->setChecked(false); });
    QObject::connect(socket_, &SocketClient::ConnectionEstablished, this, [=](){ qDebug() << "sss"; ui_->radioButton_socket_connected->setChecked(true); });
}
void ClientConnectionGui::AppendText(QByteArray data) {
    ui_->plainTextEdit_serialClient->appendPlainText(QString(data));
}

QByteArray ClientConnectionGui::GetSerialLinebreak() {
    switch (static_cast<LINEBREAK>(ui_->comboBox_linebreak->currentData().toInt())) {
    case LINEBREAK::CRLF:
        serial_linebreak_ = QByteArray("\r\n");
        break;
    case LINEBREAK::CR:
        serial_linebreak_ = QByteArray("\r");
        break;
    case LINEBREAK::LF:
        serial_linebreak_ = QByteArray("\n");
        break;
    case LINEBREAK::NONE:
        serial_linebreak_ = QByteArray("");
        break;
    default:
        qDebug() << "Unknown linebreak option";
    }
    return serial_linebreak_;
}

QByteArray ClientConnectionGui::GetSocketLinebreak() {
    switch (static_cast<LINEBREAK>(ui_->comboBox_linebreakSocketClient->currentData().toInt())) {
    case LINEBREAK::CRLF:
        socket_linebreak_ = QByteArray("\r\n");
        break;
    case LINEBREAK::CR:
        socket_linebreak_ = QByteArray("\r");
        break;
    case LINEBREAK::LF:
        socket_linebreak_ = QByteArray("\n");
        break;
    case LINEBREAK::NONE:
        socket_linebreak_ = QByteArray("");
        break;
    default:
        qDebug() << "Unknown linebreak option";
    }
    return socket_linebreak_;
}

void ClientConnectionGui::ConnectSerialPort() {
    if (serial_) {
        delete serial_;
    }

    serial_ = new SerialClient(ui_->comboBox_serialports->currentText(),
                               static_cast<QSerialPort::BaudRate>(ui_->comboBox_baudrate->currentData().toInt()),
                               static_cast<QSerialPort::DataBits>(ui_->comboBox_databits->currentData().toInt()),
                               static_cast<QSerialPort::Parity>(ui_->comboBox_parity->currentData().toInt()),
                               static_cast<QSerialPort::StopBits>(ui_->comboBox_stopbits->currentData().toInt()),
                               static_cast<QSerialPort::FlowControl>(ui_->comboBox_flowcontrol->currentData().toInt()),
                               ui_->spinBox_connectionTimeout->value(),
                               ui_->spinBox_readTimeout->value(),
                               ui_->spinBox_writeTimeout->value(),
                               GetSerialLinebreak());
    ConnectSerialSignals();
    serial_->Connect();
}

void ClientConnectionGui::ConnectSocket() {
    if (socket_) {
        delete socket_;
    }

    socket_ = new SocketClient(ui_->lineEdit_ipSocketClient->text(),
                               ui_->lineEdit_portSocketClient->text().toInt(),
                               ui_->spinBox_connectionTimeoutSocketClient->value(),
                               ui_->spinBox_readTimeoutSocketClient->value(),
                               ui_->spinBox_writeTimeoutSocketClient->value(),
                               GetSocketLinebreak());
    ConnectSocketSignals();
    socket_->Connect();
}

void ClientConnectionGui::GetAvailableSerialPorts() {
    QList<QSerialPortInfo> serial_ports = QSerialPortInfo::availablePorts();
    for (QSerialPortInfo p : serial_ports) {
        ui_->comboBox_serialports->addItem(p.portName(), p.systemLocation());
    }
}

void ClientConnectionGui::FillSerialPortOptions() {
    ui_->comboBox_baudrate->addItem("Baud1200", QSerialPort::Baud1200);
    ui_->comboBox_baudrate->addItem("Baud2400", QSerialPort::Baud2400);
    ui_->comboBox_baudrate->addItem("Baud4800", QSerialPort::Baud4800);
    ui_->comboBox_baudrate->addItem("Baud9600", QSerialPort::Baud9600);
    ui_->comboBox_baudrate->addItem("Baud19200", QSerialPort::Baud19200);
    ui_->comboBox_baudrate->addItem("Baud38400", QSerialPort::Baud38400);
    ui_->comboBox_baudrate->addItem("Baud57600", QSerialPort::Baud57600);
    ui_->comboBox_baudrate->addItem("Baud115200", QSerialPort::Baud115200);

    ui_->comboBox_databits->addItem("Data5", QSerialPort::Data5);
    ui_->comboBox_databits->addItem("Data6", QSerialPort::Data6);
    ui_->comboBox_databits->addItem("Data7", QSerialPort::Data7);
    ui_->comboBox_databits->addItem("Data8", QSerialPort::Data8);

    ui_->comboBox_parity->addItem("NoParity", QSerialPort::NoParity);
    ui_->comboBox_parity->addItem("EvenParity", QSerialPort::EvenParity);
    ui_->comboBox_parity->addItem("OddParity", QSerialPort::OddParity);
    ui_->comboBox_parity->addItem("SpaceParity", QSerialPort::SpaceParity);
    ui_->comboBox_parity->addItem("MarkParity", QSerialPort::MarkParity);

    ui_->comboBox_stopbits->addItem("OneStop", QSerialPort::OneStop);
    ui_->comboBox_stopbits->addItem("OneAndHalfStop", QSerialPort::OneAndHalfStop);
    ui_->comboBox_stopbits->addItem("TwoStop", QSerialPort::TwoStop);

    ui_->comboBox_flowcontrol->addItem("NoFlowControl", QSerialPort::NoFlowControl);
    ui_->comboBox_flowcontrol->addItem("HardwareControl", QSerialPort::HardwareControl);
    ui_->comboBox_flowcontrol->addItem("SoftwareControl", QSerialPort::SoftwareControl);

    ui_->comboBox_linebreak->addItem("CRLF", LINEBREAK::CRLF);
    ui_->comboBox_linebreak->addItem("CR", LINEBREAK::CR);
    ui_->comboBox_linebreak->addItem("LF", LINEBREAK::LF);
    ui_->comboBox_linebreak->addItem("", LINEBREAK::NONE);

    ui_->comboBox_linebreakSocketClient->addItem("CRLF", LINEBREAK::CRLF);
    ui_->comboBox_linebreakSocketClient->addItem("CR", LINEBREAK::CR);
    ui_->comboBox_linebreakSocketClient->addItem("LF", LINEBREAK::LF);
    ui_->comboBox_linebreakSocketClient->addItem("", LINEBREAK::NONE);

    ui_->spinBox_connectionTimeout->setMaximum(10000);
    ui_->spinBox_connectionTimeout->setMinimum(0);
    ui_->spinBox_readTimeout->setMaximum(10000);
    ui_->spinBox_readTimeout->setMinimum(0);
    ui_->spinBox_writeTimeout->setMaximum(10000);
    ui_->spinBox_writeTimeout->setMinimum(0);
    ui_->spinBox_connectionTimeout->setSingleStep(1000);
    ui_->spinBox_readTimeout->setSingleStep(1000);
    ui_->spinBox_writeTimeout->setSingleStep(1000);
    ui_->spinBox_connectionTimeout->setValue(5000);
    ui_->spinBox_readTimeout->setValue(2000);
    ui_->spinBox_writeTimeout->setValue(2000);

    ui_->spinBox_connectionTimeoutSocketClient->setMaximum(10000);
    ui_->spinBox_connectionTimeoutSocketClient->setMinimum(0);
    ui_->spinBox_readTimeoutSocketClient->setMaximum(10000);
    ui_->spinBox_readTimeoutSocketClient->setMinimum(0);
    ui_->spinBox_writeTimeoutSocketClient->setMaximum(10000);
    ui_->spinBox_writeTimeoutSocketClient->setMinimum(0);
    ui_->spinBox_connectionTimeoutSocketClient->setSingleStep(1000);
    ui_->spinBox_readTimeoutSocketClient->setSingleStep(1000);
    ui_->spinBox_writeTimeoutSocketClient->setSingleStep(1000);
    ui_->spinBox_connectionTimeoutSocketClient->setValue(5000);
    ui_->spinBox_readTimeoutSocketClient->setValue(2000);
    ui_->spinBox_writeTimeoutSocketClient->setValue(2000);
}


void ClientConnectionGui::WriteSerialData() {
    if (!serial_) {
        qDebug() << "No serial port configured";
        return;
    }

    if (!ui_->radioButton_serialWriteMultiple->isChecked()) {
        WriteSerial();
    } else {
        timer_.setSingleShot(false);
        timer_.setInterval(ui_->lineEdit_msSleepSerialClient->text().toInt());
        QObject::connect(&timer_, &QTimer::timeout, this, &ClientConnectionGui::WriteSerial);
        timer_.start();
    }
}

void ClientConnectionGui::WriteSerial() {
    serial_->Write(ui_->lineEdit_writeDataSerialClient->text() + serial_linebreak_);
}

void ClientConnectionGui::WriteSocketData() {
    if (!socket_) {
        qDebug() << "No socket configured";
        return;
    }
    if (!ui_->radioButton_socketWriteMultiple->isChecked()) {
        WriteSocket();
    } else {
        timer_.setSingleShot(false);
        timer_.setInterval(ui_->lineEdit_msSleepSocketClient->text().toInt());
        QObject::connect(&timer_, &QTimer::timeout, this, &ClientConnectionGui::WriteSocket);
        timer_.start();
    }
}

void ClientConnectionGui::WriteSocket() {
    socket_->Write(ui_->lineEdit_writeDataSocketClient->text() + socket_linebreak_);
}
