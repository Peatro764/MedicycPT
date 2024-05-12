#include "RadiationMonitor.h"
#include "ui_RadiationMonitor.h"

#include <QMessageBox>
#include <QProgressDialog>
#include <cstdlib>

#include "SocketClient.h"

RadiationMonitor::RadiationMonitor(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::RadiationMonitor),
    settings_(QStandardPaths::locate(QStandardPaths::ConfigLocation, QString("MRM.ini"), QStandardPaths::LocateFile),
                                                          QSettings::IniFormat)
{
    ui_->setupUi(this);
    setWindowTitle("Medicyc Radiation Monitor (MRM) v2.0.0");
    SetupRadiationGraph();
    SetupDBus();
    ConnectSignals();

    if (QFile(settings_.fileName()).exists()) {
        qDebug() << "MRM.ini found";
    } else {
        qWarning() << "MRM.ini not found";
        DisplayError("Fichier configuration introuvable: ~.config/MRM.ini ");
    }
}

RadiationMonitor::~RadiationMonitor() {
    if (radiation_level_graph_) delete radiation_level_graph_;
    QDBusConnection::disconnectFromBus(dbus_name_);
}

void RadiationMonitor::SetupDBus() {
    if (!(settings_.contains("dbus/host") && settings_.contains("dbus/port") && settings_.contains("dbus/name"))) {
        qWarning() << "MRM.ini does not contain all necessary dbus parameters (host, port, name)";
        DisplayError("Le fichier de configuration MRM.ini ne contient pas tous les paramètres nécessaires");
    }
    QString dbus_host = settings_.value("dbus/host", "").toString();
    QString dbus_port = settings_.value("dbus/port", "").toString();
    dbus_name_ = settings_.value("dbus/name", "").toString();
    QString dbus_conn = QString("tcp:host=%1,port=%2").arg(dbus_host).arg(dbus_port);
    qDebug() << "RadiationMonitor::SetupDBus "
             << "Host " << dbus_host
             << "Port " << dbus_port
             << "Name " << dbus_name_
             << "Full connection name " << dbus_conn;
    QDBusConnection dbus_connection = QDBusConnection::connectToBus(dbus_conn, dbus_name_);
    if (dbus_connection.isConnected()) {
        qDebug() << "RadiationMonitor::SetupDBus DBus connection established";
    } else {
        qWarning() << "RadiationMonitor::SetupDBus DBus connection failed";
        DisplayError("La connexion au bus dbus a échoué");
    }
    radiationmonitor_interface_ = new medicyc::cyclotron::RadiationMonitorInterface("medicyc.cyclotron.hardware.radiation_monitor.RadiationMonitor_Salle_Clinic",
                                                                                    "/RadiationMonitor_Salle_Clinic", dbus_connection, this);
    if (radiationmonitor_interface_->isValid()) {
        qDebug() << "RadiationMonitor::SetupDBus Interface connection valid";
    } else {
        qWarning() << "RadiationMonitor::SetupDBus Interface connection not valid";
        DisplayError("La connexion du client dbus a échoué");
    }
}

void RadiationMonitor::ConnectSignals() {
    QObject::connect(radiationmonitor_interface_, &medicyc::cyclotron::RadiationMonitorInterface::SIGNAL_MeasurementInProgress, radiation_level_graph_, &RadiationLevelGraph::StartRectangle);
    QObject::connect(radiationmonitor_interface_, &medicyc::cyclotron::RadiationMonitorInterface::SIGNAL_MeasurementStopped, radiation_level_graph_, &RadiationLevelGraph::StopRectangle);
    QObject::connect(radiationmonitor_interface_, &medicyc::cyclotron::RadiationMonitorInterface::SIGNAL_AbortChargeMeasurement, radiation_level_graph_, &RadiationLevelGraph::StopRectangle);
    //QObject::connect(radiationmonitor_interface_, &medicyc::cyclotron::RadiationMonitorInterface::SIGNAL_Error, this, &RadiationMonitor::DisplayError);
    QObject::connect(radiationmonitor_interface_, &medicyc::cyclotron::RadiationMonitorInterface::SIGNAL_DoseRate, this, [&]() { qDebug() << "SIGNAL DoseRate"; });
    QObject::connect(radiationmonitor_interface_, &medicyc::cyclotron::RadiationMonitorInterface::SIGNAL_DoseRate, radiation_level_graph_, &RadiationLevelGraph::AddInstantenousData);
    QObject::connect(radiationmonitor_interface_, &medicyc::cyclotron::RadiationMonitorInterface::SIGNAL_XRayDetected ,this, [&]() { DisplayInfo("Cliché détecté"); });
    QObject::connect(radiationmonitor_interface_, &medicyc::cyclotron::RadiationMonitorInterface::SIGNAL_IntegratedChargeContinous, radiation_level_graph_, &RadiationLevelGraph::AddIntegratedData);
    QObject::connect(radiationmonitor_interface_, &medicyc::cyclotron::RadiationMonitorInterface::SIGNAL_IntegratedChargeContinous, this, [&]() { qDebug() << "SIGNAL IntegratedCharge"; });
    QObject::connect(radiationmonitor_interface_, &medicyc::cyclotron::RadiationMonitorInterface::SIGNAL_IntegratedChargeOverInterval ,this, [&](double value) { qDebug() << "Measured charge: " << value; });
}

void RadiationMonitor::SetupRadiationGraph() {
    //    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
    //    effect->setBlurRadius(10);
    //    effect->setOffset(5);

    radiation_level_graph_ = new RadiationLevelGraph(ui_->raditionLevelChartView);

    radiation_level_graph_->SetWarningThreshold1(1.1E-15 * 4.32E7);
    radiation_level_graph_->SetWarningThreshold2(5.79E-13 * 4.32E7);
    radiation_level_graph_->SetWarningThreshold3(1.85E-12 * 4.32E7);
}

void RadiationMonitor::DisplayError(QString msg) {
    if (message_active_) {
        return;
    }
    message_active_ = true;
    qDebug() << "RadiationMonitor::DisplayError " << msg;
    QMessageBox* box = new QMessageBox(this);
    QObject::connect(box, &QMessageBox::finished, this, [=]{ message_active_ = false; });
    box->setText(msg);
    box->setIcon(QMessageBox::Critical);
    box->setModal(false);
    box->setAttribute(Qt::WA_DeleteOnClose);
    QTimer::singleShot(3000, box, SLOT(close()));
    box->show();
}

void RadiationMonitor::DisplayInfo(QString msg) {
    qDebug() << "RadiationMonitor::DisplayInfo " << msg;
    QMessageBox* box = new QMessageBox(this);
    box->setText(msg);
    box->setIcon(QMessageBox::Information);
    box->setModal(false);
    box->setAttribute(Qt::WA_DeleteOnClose);
    QTimer::singleShot(3000, box, SLOT(close()));
    box->show();
}


