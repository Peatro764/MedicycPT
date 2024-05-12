#include "NJ1Gui.h"
#include "ui_NJ1Gui.h"

#include <QMessageBox>
#include <QStandardPaths>
#include <QDebug>
#include <unistd.h>

NJ1Gui::NJ1Gui(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::NJ1Gui),
    settings_(std::unique_ptr<QSettings>(new QSettings(QStandardPaths::locate(QStandardPaths::ConfigLocation, QString("Cyclotron.ini"), QStandardPaths::LocateFile),
                       QSettings::IniFormat))),
  nj1_("MTD", true)
{
    ui_->setupUi(this);

    QObject::connect(&nj1_, &nj1::NJ1::SIGNAL_Connected, this, [&]() { ui_->radioButton_connected->setChecked(true); });
    QObject::connect(&nj1_, &nj1::NJ1::SIGNAL_Disconnected, this, [&]() { ui_->radioButton_connected->setChecked(false); });
    QObject::connect(&nj1_, &nj1::NJ1::SIGNAL_IOError, this, [&](QString error) { ui_->plainTextEdit_io->appendPlainText(QString("IO ERROR: ") + error); });

    QObject::connect(ui_->pushButton_connect, &QPushButton::clicked, this, &NJ1Gui::Connect);
    QObject::connect(ui_->pushButton_disconnect, &QPushButton::clicked, this, &NJ1Gui::Disconnect);
    QObject::connect(ui_->pushButton_upload_seance, &QPushButton::clicked, this, &NJ1Gui::UploadSeance);
    QObject::connect(ui_->pushButton_ack_seance_started, &QPushButton::clicked, &nj1_, &nj1::NJ1::AcknowledgeSeanceStart);
    QObject::connect(ui_->pushButton_ack_seance_finished, &QPushButton::clicked, &nj1_, &nj1::NJ1::AcknowledgeSeanceFinish);
    QObject::connect(&nj1_, &nj1::NJ1::SIGNAL_SeanceStartedPacket, this, [&]() {
        if (ui_->radioButton_sendAckStartAuto->isChecked()) {
            if (ui_->radioButton_seanceStartedAckCorrupt->isChecked()) nj1_.AcknowledgeSeanceFinish(); else  nj1_.AcknowledgeSeanceStart(); }});
    QObject::connect(&nj1_, &nj1::NJ1::SIGNAL_SeanceFinishedPacket, this, [&]() {
        if (ui_->radioButton_sendAckFinishAuto->isChecked()) {
            if (ui_->radioButton_seanceFinishedAckCorrupt->isChecked()) nj1_.AcknowledgeSeanceStart(); else nj1_.AcknowledgeSeanceFinish(); } });
}

NJ1Gui::~NJ1Gui() {

}

void NJ1Gui::UploadSeance() {
    try {
        SeanceConfig seance_config(ui_->lineEdit_uploadseance_name->text(),
                           ui_->lineEdit_uploadSeance_dossier->text().toInt(),
                           ui_->lineEdit_uploadSeance_seanceId->text().toInt(),
                                   13.0,
                           ui_->lineEdit_uploadSeance_debit->text().toFloat(),
                           ui_->lineEdit_uploadSeance_mu_des->text().toInt(),
                           ui_->lineEdit_uploadSeance_mu_cor->text().toInt(),
                           (int)(ui_->lineEdit_uploadSeance_duration->text().toDouble()*1000),
                           ui_->lineEdit_uploadSeance_stripper->text().toInt());
        nj1_.UploadNewSeanceConfig(seance_config);
    } catch (std::exception& exc) {
        QMessageBox::warning(this, "Write error", QString::fromStdString(exc.what()));
    }
}

void NJ1Gui::Disconnect() {
//    try {
//        omron_.Disconnect();
//        ui_->radioButton_connected->setChecked(false);
//    } catch (std::exception& exc) {
//        QMessageBox::warning(this, "Error", QString::fromStdString(exc.what()));
//    }
}

void NJ1Gui::Connect() {
//    try {
//        omron::OmronConfig config(omron::OmronConfig::OmronType::CS,
//                                  ui_->lineEdit_ip->text(),
//                                  ui_->lineEdit_port->text().toInt());
//        omron_.SetConfig(config);
//        omron_.Connect();
//        ui_->radioButton_connected->setChecked(true);
//    } catch (std::exception& exc) {
//        QMessageBox::warning(this, "Error", QString::fromStdString(exc.what()));
//    }
}

