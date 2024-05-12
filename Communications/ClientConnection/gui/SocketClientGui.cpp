#include "SocketIOGui.h"
#include "ui_SocketIOGui.h"


SocketIOGui::SocketIOGui(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::SocketIOGui),
    socket_client_("localhost", 3333, 1000, 1000, 1000, "d\n")
{
    ui_->setupUi(this);
    QObject::connect(&socket_client_, SIGNAL(Message(QString)), ui_->messagePlainTextEdit, SLOT(appendPlainText(QString)));
    QObject::connect(&socket_client_, SIGNAL(SocketError(QString)), ui_->messagePlainTextEdit, SLOT(appendPlainText(QString)));
    QObject::connect(ui_->connectPushButton, SIGNAL(clicked()), &socket_client_, SLOT(Connect()));
    QObject::connect(ui_->writeDataLineEdit, SIGNAL(returnPressed()) , this, SLOT(WriteData()));
    QObject::connect(&socket_client_, SIGNAL(WriteError(QString)), ui_->messagePlainTextEdit, SLOT(appendPlainText(QString)));
    QObject::connect(&socket_client_, SIGNAL(ReadError(QString)), ui_->messagePlainTextEdit, SLOT(appendPlainText(QString)));
    QObject::connect(&socket_client_, SIGNAL(DataWritten(QString)), ui_->messagePlainTextEdit, SLOT(appendPlainText(QString)));
    QObject::connect(&socket_client_, SIGNAL(DataRead(QString)), ui_->messagePlainTextEdit, SLOT(appendPlainText(QString)));
}

SocketIOGui::~SocketIOGui()
{
}


void SocketIOGui::WriteData() {
    const int n(ui_->writeSpinBox->value());
    for (int i = 0; i < n; ++i) {
        socket_client_.Write(ui_->writeDataLineEdit->text());
    }
}
