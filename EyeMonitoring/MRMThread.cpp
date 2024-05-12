#include "MRMThread.h"

#include <QStandardPaths>
#include <QDebug>
#include <QSettings>
#include <QtDBus/QDBusConnection>

#include "radiationmonitor_interface.h"

MRMThread::MRMThread(QObject* parent) : QThread(parent)
{}

MRMThread::~MRMThread() {
}

void MRMThread::run() {
    qDebug() << "MRMThread::run " << QThread::currentThreadId();
    QSettings settings(QStandardPaths::locate(QStandardPaths::ConfigLocation, QString("MEM.ini"), QStandardPaths::LocateFile),
                       QSettings::IniFormat);

    QString dbus_host = settings.value("dbus/host", "").toString();
    QString dbus_port = settings.value("dbus/port", "").toString();
    QString dbus_name = settings.value("dbus/name", "").toString();
    QString dbus_conn = QString("tcp:host=%1,port=%2").arg(dbus_host).arg(dbus_port);
    qDebug() << "MRMThread::run "
             << "Host " << dbus_host
             << "Port " << dbus_port
             << "Name " << dbus_name
             << "Full connection name " << dbus_conn;

    QDBusConnection dbus_connection = QDBusConnection::connectToBus(dbus_conn, dbus_name);
    if (dbus_connection.isConnected()) {
        qDebug() << "MRMThread::run  Dbus connected";
    } else {
        qWarning() << "MRMThread::run  Dbus not connected";
    }

    medicyc::cyclotron::RadiationMonitorInterface rm_dbus("medicyc.cyclotron.hardware.radiation_monitor.RadiationMonitor_Salle_Clinic",
                                                          "/RadiationMonitor_Salle_Clinic", dbus_connection, this);
    if (rm_dbus.isValid()) {
        qDebug() << "MRMThread::run  RadiationMonitorInterface valid";
    } else {
        qWarning() << "MRMThread::run  RadiationMonitorInterface not valid";
    }

    QObject::connect(&rm_dbus, &medicyc::cyclotron::RadiationMonitorInterface::SIGNAL_XRayDetected, this, &MRMThread::SIGNAL_XRayDetected);
    QObject::connect(&rm_dbus, &medicyc::cyclotron::RadiationMonitorInterface::SIGNAL_MeasurementInProgress, this, &MRMThread::SIGNAL_IrradiationStarted);
    QObject::connect(&rm_dbus, &medicyc::cyclotron::RadiationMonitorInterface::SIGNAL_MeasurementStopped, this, &MRMThread::SIGNAL_IrradiationStopped);
    exec(); // This will block the thread until its final gruesome death

    qDebug() << "MRMThread exiting";
}
