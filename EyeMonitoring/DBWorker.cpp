#include "DBWorker.h"

#include <QDebug>

DBWorker::DBWorker(QObject* parent)
    : QObject(parent),
      settings_(std::unique_ptr<QSettings>(new QSettings(QStandardPaths::locate(QStandardPaths::ConfigLocation, QString("MEM.ini"), QStandardPaths::LocateFile),
                                                            QSettings::IniFormat))) {
}

DBWorker::~DBWorker() {
    qDebug() << "~DBWorker";
}

void DBWorker::Setup() {
    qDebug() << "DBWorker::Setup " << QThread::currentThreadId();
    QString hostName = settings_->value("database/host", "").toString();
    QString databaseName = settings_->value("database/name", "").toString();
    int port = settings_->value("database/port", "0").toInt();
    QString userName = settings_->value("database/user", "").toString();
    QString password = settings_->value("database/password", "").toString();
    QString connName = "db";

    if (hostName.isEmpty() || databaseName.isEmpty() || !port || userName.isEmpty()
            || password.isEmpty() || connName.isEmpty()) {
        if (emit_messages_) {
            emit_messages_ = false;
            emit SIGNAL_Error("Impossible de trouver les paramètres de connexion de la base de données");
        }
        return;
    }
    if (repo_) repo_.reset();
    repo_ = std::unique_ptr<PTRepo>(new PTRepo(connName, hostName, port, databaseName, userName, password));
    if (repo_->Connect()) {
        qDebug() << "DBWorker::Setup Connection successful";
        emit_messages_ = true;
    } else {
        if (emit_messages_) {
            emit_messages_ = false;
            emit SIGNAL_Error("Échec de la connexion à la base de données");
        }
        QTimer::singleShot(5000, this, &DBWorker::Setup);
    }
}

void DBWorker::SaveXRayRecord(int dossier, double peak_rate, double integrated_charge) {
    qDebug() << "DBWorker::SaveXRayRecord " << QThread::currentThreadId();
    if (!repo_) {
        emit SIGNAL_Error("Échec de l'enregistrement de la radiographie. Pas de connexion à la base de données");
        return;
    }
    try {
        repo_->SaveXRayRecord(XRayRecord(QDateTime::currentDateTime(), dossier,
                                         peak_rate, integrated_charge));
        qDebug() << "DBWorker::SaveXRayRecord Successfully saved to db";
    } catch (std::exception& exc) {
        emit SIGNAL_Error("Échec de l'enregistrement de la radiographie dans la base de données");
        QTimer::singleShot(2000, this, &DBWorker::Setup);
    }
}

void DBWorker::GetPatient(int dossier) {
    qDebug() << "DBWorker::GetPatient " << QThread::currentThreadId();
    if (!repo_) {
        emit SIGNAL_Error("Échec de la connexion à la base de données");
        return;
    }
    try {
        if (repo_->DossierExist(dossier)) {
            const Patient patient = repo_->GetPatient(dossier);
            emit SIGNAL_Patient(patient);
        } else {
            emit SIGNAL_Error("Le patient n'existe pas dans la base de données");
            return;
        }
    } catch (std::exception& exc) {
        qWarning() << "DBWorker::GetPatient Exception thrown: " << exc.what();
        emit SIGNAL_Error("Échec de la connexion à la base de données");
        QTimer::singleShot(2000, this, &DBWorker::Setup);
    }
}

