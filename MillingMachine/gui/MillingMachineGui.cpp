#include "MillingMachineGui.h"
#include "ui_MillingMachineGui.h"

#include <QDebug>
#include <QFileDialog>
#include <QtSerialPort/QSerialPortInfo>
#include <QSettings>
#include <QStandardPaths>
#include <QMessageBox>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
#include <QPainter>
#include <QGraphicsDropShadowEffect>

#include "FetchProgramDialog.h"
#include "SaveProgramDialog.h"
#include "ProgramTypeDialog.h"
#include "Coordinate.h"
#include "MillingMachineIODialog.h"
#include "ConfigDialog.h"

MillingMachineGui::MillingMachineGui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MillingMachineGui),
    settings_(QStandardPaths::locate(QStandardPaths::ConfigLocation, QString("MMM.ini"), QStandardPaths::LocateFile),
                                                          QSettings::IniFormat),
    repo_(nullptr),
    serial_communication_(false)
{
    ui->setupUi(this);
    setWindowTitle(QString("MMM (Medicyc Milling Machine) v1.1.3"));
    TurnOffDbButton();

    ui->dossierLabel->setText("Id");
    ui->fraiseuseProgramPlainTextEdit->setGraphicsEffect(new QGraphicsDropShadowEffect);

    QObject::connect(ui->quitPushButton, SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect(ui->dbPushButton, SIGNAL(clicked()), this, SLOT(SetupDb()));
    QObject::connect(ui->dbPushButton, SIGNAL(clicked()), this, SLOT(FillProgramTypes()));
    QObject::connect(ui->configPushButton, SIGNAL(clicked()), this, SLOT(LaunchConfigDialog()));
    QObject::connect(ui->printPushButton, SIGNAL(clicked()), this, SLOT(LaunchPrinterDialog()));
    QObject::connect(ui->createTestProgramPushButton, SIGNAL(clicked()), this, SLOT(CreateTestProgram()));
    QObject::connect(ui->sendPushButton, SIGNAL(clicked()), this, SLOT(SendProgram()));
    QObject::connect(ui->readPushButton , SIGNAL(clicked()), this, SLOT(ReadProgram()));
    QObject::connect(ui->sendToFilePushButton, SIGNAL(clicked()), this, SLOT(LaunchSaveFileDialog()));
    QObject::connect(ui->readFromDatabasePushButton, SIGNAL(clicked()), this, SLOT(ReadProgramFromDatabase()));
    QObject::connect(ui->sendToDatabasePushButton , SIGNAL(clicked()), this, SLOT(SaveProgramToDatabase()));
    QObject::connect(&milling_machine_, SIGNAL(ProgramSent()), this, SLOT(WriteSuccess()));
    QObject::connect(&milling_machine_, SIGNAL(SendError(QString)), this, SLOT(WriteError(QString)));
    QObject::connect(ui->serialPortPushButton, SIGNAL(clicked()), this, SLOT(SwitchToSerialPort()));
    QObject::connect(ui->ethernetPortPushButton, SIGNAL(clicked()), this, SLOT(SwitchToEthernetPort()));

    CreateActions();
    CreateTrayIcon();
    SetupDb();
    FillProgramTypes();
    SwitchToEthernetPort();
}

MillingMachineGui::~MillingMachineGui()
{
    if (repo_) delete repo_;
}

void MillingMachineGui::ShowAllMessages() {
    QString messages(messages_.empty() ? QString("None") : messages_.join("\n"));
    tray_icon_->showMessage("New milling programs", messages,
                            QSystemTrayIcon::Information, 10000);
}

void MillingMachineGui::ClearMessages() {
    tray_icon_->setIcon(QIcon(":/images/fraiseuse_green_transparant.png"));
    messages_.clear();
}

void MillingMachineGui::CreateActions() {
    show_msg_action_ = new QAction(tr("Show messages"), this);
    connect(show_msg_action_, &QAction::triggered, this, [this](){ ShowAllMessages(); });

    clear_msg_action_ = new QAction(tr("Clear messages"), this);
    connect(clear_msg_action_, &QAction::triggered, this, [this](){ ClearMessages(); });

    minimize_action_ = new QAction(tr("Mi&nimize"), this);
    connect(minimize_action_, &QAction::triggered, this, &QWidget::hide);

    maximize_action_ = new QAction(tr("Ma&ximize"), this);
    connect(maximize_action_, &QAction::triggered, this, &QWidget::showMaximized);

    restore_action_ = new QAction(tr("&Restore"), this);
    connect(restore_action_, &QAction::triggered, this, &QWidget::showNormal);

    quit_action_ = new QAction(tr("&Quit"), this);
    connect(quit_action_, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void MillingMachineGui::CreateTrayIcon() {
    tray_icon_menu_ = new QMenu(this);
    tray_icon_menu_->addAction(show_msg_action_);
    tray_icon_menu_->addAction(clear_msg_action_);
    tray_icon_menu_->addSeparator();
    tray_icon_menu_->addAction(minimize_action_);
    tray_icon_menu_->addAction(maximize_action_);
    tray_icon_menu_->addAction(restore_action_);
    tray_icon_menu_->addSeparator();
    tray_icon_menu_->addAction(quit_action_);

    tray_icon_ = new QSystemTrayIcon(this);
    tray_icon_->setIcon(QIcon(":/images/fraiseuse_green_transparant.png"));
    tray_icon_->setToolTip("Fraiseuse");
    tray_icon_->setContextMenu(tray_icon_menu_);
    tray_icon_->show();
}

void MillingMachineGui::HandleNotification(QString channel, QSqlDriver::NotificationSource, QVariant message) {
    (void)channel;
    QString message_timestamped(message.toString() + " \t" + QDateTime::currentDateTime().toString());
    messages_.push_back(message_timestamped);
    tray_icon_->setIcon(QIcon(":/images/fraiseuse_red_transparant.png"));
    if(!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::warning(this, tr("Fraiseuse"), message_timestamped);
    } else {
        qDebug() << "Notification " << message_timestamped;
        if (tray_icon_) tray_icon_->showMessage("New milling program", message_timestamped,
						QSystemTrayIcon::Information, 10000);
    }
}

void MillingMachineGui::SetupDb() {
    if (repo_) {
        delete repo_;
        repo_ = nullptr;
    }
    QString dbHostName = settings_.value("ptdatabase/host", "unknown").toString();
    QString dbDatabaseName = settings_.value("ptdatabase/name", "unknown").toString();
    int dbPort = settings_.value("ptdatabase/port", "5432").toInt();
    QString dbUserName = settings_.value("ptdatabase/user", "unknown").toString();
    QString dbPassword = settings_.value("ptdatabase/password", "unknown").toString();
    QString dbConnName = "ptdb";
    int deconnect_on_idle_timeout = settings_.value("ptdatabase/disconnect_on_idle_timeout", "600").toInt();

    if (dbHostName == "unknown") qCritical() << "MTP::SetupDb Unknown db hostname";
    if (dbDatabaseName == "unknown") qCritical() << "MTP::SetupDb Unknown db name";
    if (dbUserName == "unknown") qCritical() << "MTP::SetupDb Unknown db username";
    if (dbPassword == "unknown") qCritical() << "MTP::SetupDb Unknown db password";

    repo_ = new PTRepo(dbConnName, dbHostName, dbPort, dbDatabaseName, dbUserName, dbPassword, deconnect_on_idle_timeout);
    QObject::connect(repo_, SIGNAL(SIGNAL_ConnectionClosed()), this, SLOT(TurnOffDbButton()));
    QObject::connect(repo_, SIGNAL(SIGNAL_ConnectionOpened()), this, SLOT(TurnOnDbButton()));
    QObject::connect(repo_, SIGNAL(SIGNAL_FailedOpeningConnection()), this, SLOT(FailedConnectingToDatabase()));
    QObject::connect(repo_, SIGNAL(SIGNAL_FailedOpeningConnection()), this, SLOT(TurnOffDbButton()));

    repo_->Connect();

    if (repo_->SubscribeToNotification("fraiseuse_notification")) {
        QObject::connect(repo_, SIGNAL(Notification(QString,QSqlDriver::NotificationSource,QVariant)), this, SLOT(HandleNotification(QString, QSqlDriver::NotificationSource, QVariant)));
        qDebug() << "MTP::SetupDB Notifications subscription ok";
    } else {
      qWarning() << "MTP::SetupDB Notifications subscription failed";
    }
}

void MillingMachineGui::LaunchConfigDialog() {
    ConfigDialog dialog(this);
    dialog.exec();
}

void MillingMachineGui::LaunchSaveFileDialog() {
    QString filename(QFileDialog::getSaveFileName(this, "Sauvegarde dans un fichier",
                                                  "/home/hofverberg/", "*.mil"));
    if (!filename.isNull() && !filename.isEmpty()) {
        milling_machine_.SetProgram(ui->fraiseuseProgramPlainTextEdit->toPlainText());
        milling_machine_.SendProgramToFile(filename);
    }
}

void MillingMachineGui::LaunchPrinterDialog() {
    try {
        QPrinter printer;
        QPrintDialog dialog(&printer, this);
        dialog.setWindowTitle(tr("Imprimer programme fraiseuse"));
        if (dialog.exec() != QDialog::Accepted)
            return;
        ui->fraiseuseProgramPlainTextEdit->print(&printer);
    }
    catch (std::exception const& exc) {
        qWarning() << "MillingMachineGui::LaunchPrinterDialog Exception thrown: " << exc.what();
        QMessageBox::warning(this, tr("Fraiseuse"), "Erreur d'impression");
    }
}

void MillingMachineGui::WriteError(QString error) {
    QMessageBox::warning(this, "Fraiseuese", error);
}

void MillingMachineGui::WriteSuccess() {
     QMessageBox::information(this, "Fraiseuese", "Programme correctement écrit");
}

void MillingMachineGui::CreateTestProgram() {
    milling_machine_.CreateMinimalistCollimatorProgram();
    ui->fraiseuseProgramPlainTextEdit->setPlainText(milling_machine_.GetProgram());
}

void MillingMachineGui::SendProgram() {
    milling_machine_.SetProgram(ui->fraiseuseProgramPlainTextEdit->toPlainText());
    if (serial_communication_) {
        milling_machine_.SendProgramOverSerialPort();
    } else {
        milling_machine_.SendProgramOverNetwork();
    }
}

void MillingMachineGui::ReadProgram() {
    DataReader *reader = serial_communication_ ? milling_machine_.GetSerialPortReader() :
                                                 milling_machine_.GetSocketReader();
    MillingMachineIODialog dialog(this, reader);
    if (dialog.exec() == QDialog::Accepted) {
        QString program(dialog.GetData());
        qDebug() << "Program " << program;
        int pgm_start_index(program.indexOf("0 BEGIN PGM"));
        if (pgm_start_index == -1) {
            qWarning() << "MillingMachineGui::ReadProgram Read program does not contain 0 BEGIN PGM";
            UpdateProgram("?", "?", QString("Read program does not contain 0 BEGIN PGM"));
        } else {
            QString stripped_program(program.right(program.size() - pgm_start_index));
            milling_machine_.SetProgram(stripped_program);
            UpdateProgram(QString::number(milling_machine_.GetDossier()), "?", stripped_program);
        }
    }
}

void MillingMachineGui::SendProgramToStdOut() {
    milling_machine_.SendProgramToStdOut();
}

void MillingMachineGui::SaveProgramToDatabase() {
    if (!repo_->CheckConnection()) { return; }
    if (ui->dossierLineEdit->text().isEmpty() || ui->dossierLineEdit->text().isNull()) {
        QMessageBox::information(this, "Fraiseuse", "Donne un id au program svp");
        return;
    }

    milling_machine_.SetProgram(ui->fraiseuseProgramPlainTextEdit->toPlainText());
    QString program_type(ui->typeComboBox->currentText());
    QString program_name(ui->dossierLineEdit->text());

    try {
        if (misc_types_.contains(program_type)) {
            repo_->SaveMiscFraiseuseProgram(program_name, program_type, QDateTime::currentDateTime(), milling_machine_.GetProgram());
        } else if (treatment_types_.contains(program_type)) {
            bool ok(false);
            int dossier(program_name.toInt(&ok));
            if (!ok) { throw std::runtime_error("Nom de dossier incorrect"); }
            repo_->SaveTreatmentFraiseuseProgram(dossier, program_type, QDateTime::currentDateTime(), milling_machine_.GetProgram());
        } else {
            QMessageBox::warning(this, "Fraiseuse", "Unknown program type");
            return;
        }
        QMessageBox::information(this, "Fraiseuse", "Programme sauvé dans la base de donnéees");
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Fraiseuse", "Erreur sauvegarde dans la base de donnéees");
        qWarning() << "MillingMachineGui::SaveProgramToDatabase Exception: " + QString(exc.what());
    }
}

void MillingMachineGui::ReadProgramFromDatabase() {
    if (!repo_->CheckConnection()) { return; }
    FetchProgramDialog dialog(this, repo_);
    if (dialog.exec() == QDialog::Accepted) {
        UpdateProgram(dialog.Id(), dialog.ProgramType(), dialog.Program());
    }
}

void MillingMachineGui::UpdateProgram(QString id, QString type, QString program) {
    ui->dossierLineEdit->setText(id);
    ui->typeComboBox->setCurrentText(type);
    ui->fraiseuseProgramPlainTextEdit->setPlainText(program);
    milling_machine_.SetProgram(program);
}

QString MillingMachineGui::ButtonStyleSheet(QString image) {
    QString str ("QPushButton {"
                 "background-image: url(:/images/" + image + ");"
    "background-repeat: no-repeat;"
    "background-position: center;"
    "border-style: raised; "
    "border-width: 0px;"
    "border-radius: 25px;"
    "};"
    "QPushButton:pressed {"
    "border-width: 4px."
    "};");
    return str;
}

void MillingMachineGui::SwitchToSerialPort() {
    serial_communication_ = true;
    ui->serialPortPushButton->setStyleSheet(ButtonStyleSheet("serial_port_blue.png"));
    ui->ethernetPortPushButton->setStyleSheet(ButtonStyleSheet("ethernet_port_black.png"));
}

void MillingMachineGui::SwitchToEthernetPort() {
    serial_communication_ = false;
    ui->serialPortPushButton->setStyleSheet(ButtonStyleSheet("serial_port_black.png"));
    ui->ethernetPortPushButton->setStyleSheet(ButtonStyleSheet("ethernet_port_blue.png"));
}

void MillingMachineGui::TurnOffDbButton() {
    ui->dbPushButton->setStyleSheet(ButtonStyleSheet("db_button_red"));
}

void MillingMachineGui::TurnOnDbButton() {
    ui->dbPushButton->setStyleSheet(ButtonStyleSheet("db_button_green"));
}

void MillingMachineGui::FillProgramTypes() {
    try {
        if (!repo_->CheckConnection()) { return; }
        treatment_types_.clear();
        misc_types_.clear();
        treatment_types_ = repo_->GetFraiseuseTreatmentTypes();
        misc_types_ = repo_->GetFraiseuseMiscTypes();
        ui->typeComboBox->addItems(treatment_types_);
        ui->typeComboBox->addItems(misc_types_);
    }
    catch (std::exception& exc) {
         qWarning() << "MillingMachineGui::FillProgramTypes Exception thrown: " << exc.what();
    }
}

void MillingMachineGui::FailedConnectingToDatabase() {
    QMessageBox::critical(this, "Fraiseuese", "Echec de la connexion à la base de données");
}

