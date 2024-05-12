#include "PTRepo.h"

#include <iostream>
#include <QtDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <stdexcept>
#include <QSqlDriver>
#include <QMetaMethod>

#include "Constants.h"
#include "DbUtil.h"

PTRepo::PTRepo(const QString& conn_name,
                           const QString& hostname,
                           const int& port,
                           const QString& db_name,
                           const QString& username,
                           const QString& password)
    : PTRepo(conn_name, hostname, port, db_name, username, password, 0) {}

PTRepo::PTRepo(const QString& conn_name,
                           const QString& hostname,
                           const int& port,
                           const QString& db_name,
                           const QString& username,
                           const QString& password,
                           const int& disconnect_on_idle_timeout)
    : db_(new QSqlDatabase(QSqlDatabase::addDatabase("QPSQL7", conn_name))), conn_name_(conn_name), disconnect_on_idle_timeout_(disconnect_on_idle_timeout) {
    db_->setHostName(hostname);
    db_->setPort(port);
    db_->setDatabaseName(db_name);
    db_->setUserName(username);
    db_->setPassword(password);
    QObject::connect(db_->driver(), SIGNAL(notification(QString,QSqlDriver::NotificationSource,QVariant)),
            this, SIGNAL(Notification(QString,QSqlDriver::NotificationSource, QVariant)));
    if (disconnect_on_idle_timeout_ > 0) {
        disconnect_timer_.setInterval(disconnect_on_idle_timeout * 1000);
        QObject::connect(&disconnect_timer_, &QTimer::timeout, this, &PTRepo::Disconnect);
        disconnect_timer_.start();
    }
}

PTRepo::~PTRepo() {
    this->Disconnect();
    if (db_) delete db_;
    QSqlDatabase::removeDatabase(conn_name_);
}

bool PTRepo::Connect() {
    if (db_->isOpen()) {
        return true;
    } else {
        emit (SIGNAL_OpeningConnection());
        db_->setConnectOptions("connect_timeout=5"); //;keepalives=1;keepalives_idle=10;keepalives_interval=3;keepalives_count=3");
        if (db_->open()) {
            emit (SIGNAL_ConnectionOpened());
            qDebug() << "Connection to db established";
            return true;
        } else {
            emit (SIGNAL_FailedOpeningConnection());
            qCritical() << db_->lastError().text();
            return false;
        }
    }
}

void PTRepo::Disconnect() {
    if (db_->isOpen()) {
        db_->close();
        emit (SIGNAL_ConnectionClosed());
    }
}

bool PTRepo::SubscribeToNotification(QString channel) {
    return db_->driver()->subscribeToNotification(channel);
}

bool PTRepo::UnsubscribeFromNotification(QString channel) {
  return db_->driver()->unsubscribeFromNotification(channel);
} 
  
bool PTRepo::DossierExist(const int& dossier) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT dossier FROM Treatment WHERE dossier = :dossier");
    query.bindValue(":dossier", dossier);

    if (!query.exec()) {
        qWarning() << "Query for Dossier failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }
    return query.next();
}

bool PTRepo::PatientExist(const int& patient_id) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT patient_id from Patient where patient_id = :patient_id");
    query.bindValue(":patient_id", patient_id);
    if (!query.exec()) {
        qWarning() << "Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }
    return query.next();
}

Patient PTRepo::GetPatient(const int& dossier) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT patient_id FROM Treatment"
                  " WHERE dossier = :dossier");
    query.bindValue(":dossier", dossier);

    if (!query.exec()) {
        qWarning() << "Query for Treatment by dossier number failed: " << query.lastError();
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    if (!query.next()) {
        qWarning() << "Treatment for dossier " << dossier << " not found";
        QString message = QString("il n'y a pas de patient dans la base de données avec dossier %1").arg(dossier);
        throw std::runtime_error("Db query returned no data");
    }

    return this->GetPatientById(query.value(0).toInt());
}

std::vector<Patient> PTRepo::GetPatients(const QString &firstName, const QString &lastName) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT patient_id, first_name, last_name FROM patient"
                  " WHERE first_name ILIKE :first_name"
                  " AND last_name ILIKE :last_name");
    query.bindValue(":first_name", firstName + "%");
    query.bindValue(":last_name", lastName + "%");

    if (!query.exec()) {
        qWarning() << "PTRepo::GetPatients Query failed: " << query.lastError();
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    std::vector<Patient> patients;
    while (query.next()) {
        patients.push_back(Patient(query.value(0).toInt(),
                                   query.value(1).toString(),
                                   query.value(2).toString()));
    }
    return patients;
}

Patient PTRepo::GetPatientById(const int& patient_id) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT first_name, last_name from Patient"
                  " WHERE patient_id = :patient_id");
    query.bindValue(":patient_id", patient_id);

    if (!query.exec()) {
        qWarning() << "Query for Patient failed: " << query.lastError();
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    if (!query.next()) {
        qWarning() << "Patient with id " << patient_id << " not found";
        throw std::runtime_error("Db query returned no data");
    }

    return Patient(patient_id,
                   query.value(0).toString(),
                   query.value(1).toString());
}

int PTRepo::SavePatient(const Patient& patient) {
    QSqlQuery query(*db_);
    query.prepare("INSERT INTO Patient(first_name,last_name)"
                  " VALUES(:first_name,:last_name)"
                  " RETURNING patient_id");
    query.bindValue(":first_name", patient.GetFirstName());
    query.bindValue(":last_name", patient.GetLastName());

    if (!query.exec()) {
        qWarning() << "PTRepo::SavePatient Query failed: " << query.lastError();
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    if (!query.next()) {
        qWarning() << "PTRepo::SavePatient Query didn't return autoincrement id";
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    int patient_id = query.value(0).toInt();
    return patient_id;
}

void PTRepo::UpdatePatient(const Patient& patient) {
    QSqlQuery query(*db_);
    query.prepare("UPDATE Patient SET first_name = :first_name, last_name = :last_name"
                  " WHERE patient_id = :patient_id");
    query.bindValue(":first_name", patient.GetFirstName());
    query.bindValue(":last_name", patient.GetLastName());
    query.bindValue(":patient_id", patient.GetId());

    if (!query.exec()) {
        qWarning() << "PTRepo::UpdatePatient Query failed: " << query.lastError();
        throw std::runtime_error(query.lastError().text().toStdString());
    }
}


void PTRepo::CreateTreatment(const int& dossier, const int& patient_id, const std::vector<double> &dose_prescribed,
                                   const int& modulateur_id, const int& pri_degradeur_id, QString treatment_type) {
    CreateTreatment(dossier, patient_id, dose_prescribed, modulateur_id, pri_degradeur_id, -1, treatment_type);
}


void PTRepo::CreateTreatment(const int& dossier, const int& patient_id, const std::vector<double> &dose_prescribed,
                                   const int& modulateur_id, const int& pri_degradeur_id, const int& sec_degradeur_id, QString treatment_type) {
    try {
        db_->transaction();
        QSqlQuery query(*db_);
        if (sec_degradeur_id >= 0) {
            query.prepare("INSERT INTO Treatment(dossier, timestamp, patient_id, active_seance, modulateur_id, pri_degradeur_id, sec_degradeur_id, treatment_type)"
                          " VALUES(:dossier, :timestamp, :patient_id, :active_seance, :modulateur_id, :pri_degradeur_id, :sec_degradeur_id, :treatment_type)");
            query.bindValue(":sec_degradeur_id", sec_degradeur_id);
        } else {
            query.prepare("INSERT INTO Treatment(dossier, timestamp, patient_id, active_seance, modulateur_id, pri_degradeur_id, treatment_type)"
                          " VALUES(:dossier, :timestamp, :patient_id, :active_seance, :modulateur_id, :pri_degradeur_id, :treatment_type)");
        }
        query.bindValue(":dossier", dossier);
        query.bindValue(":timestamp", QDateTime::currentDateTime());
        query.bindValue(":patient_id", patient_id);
        query.bindValue(":active_seance", 0);
        query.bindValue(":modulateur_id", modulateur_id);
        query.bindValue(":pri_degradeur_id", pri_degradeur_id);
        query.bindValue(":treatment_type", treatment_type);

        if (!query.exec()) {
            qWarning() << "PTRepo::CreateTreatment Query failed: " << query.lastError();
            throw std::runtime_error(query.lastError().text().toStdString());
        }

        for (auto dose : dose_prescribed) {
            CreateSeance(dossier, dose);
        }

        if (!db_->commit()) {
            qWarning() << "PTRepo::CreateTreatment Commit failed";
            throw std::runtime_error(query.lastError().text().toStdString());
        }
    }
    catch (...) {
        qWarning() << "PTRepo::CreateTreatment Rolling back...";
        db_->rollback();
        throw;
    }
}

std::vector<int> PTRepo::GetDossiers(QDate from_date, QDate to_date, QString dossier, QString first_name, QString last_name, QString treatment_type) {
    QSqlQuery query(*db_);
    QString query_string("SELECT dossier FROM treatment INNER JOIN patient ON (treatment.patient_id = patient.patient_id) "
                         "WHERE Date(timestamp) >= :from_date AND Date(timestamp) <= :to_date AND "
                         "first_name ILIKE :first_name AND last_name ILIKE :last_name AND ");
    if (treatment_type != QString("Tout") && treatment_type != QString("")) {
        query_string.append("treatment_type = :treatment_type AND ");
    }
    query_string.append("CAST(dossier AS TEXT) LIKE :dossier "
                        "ORDER BY timestamp DESC");
    query.prepare(query_string);
    query.bindValue(":to_date", to_date);
    query.bindValue(":from_date", from_date);
    query.bindValue(":first_name", first_name + "%");
    query.bindValue(":last_name", last_name + "%");
    if (treatment_type != QString("Tout") && treatment_type != QString("")) {
        query.bindValue(":treatment_type", treatment_type);
    }
    query.bindValue(":dossier", dossier + "%");

    if (!query.exec()) {
        qWarning() << "PTRepo::GetDossiers Query failed: " << query.lastError();
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    std::vector<int> dossiers;
    while (query.next()) {
        dossiers.push_back(query.value(0).toInt());
    }

    return dossiers;
}

std::vector<int> PTRepo::GetNonFinishedDossiers() {
    QSqlQuery query(*db_);
    query.prepare("SELECT dossier FROM treatment WHERE active_seance != -1");

    if (!query.exec()) {
        qWarning() << "PTRepo::GetNonFinishedDossiers Query failed: " << query.lastError();
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    std::vector<int> dossiers;
    while (query.next()) {
        dossiers.push_back(query.value(0).toInt());
    }

    return dossiers;
}

Treatment PTRepo::GetTreatment(const int& dossier) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT dossier, patient_id, active_seance, modulateur_id, pri_degradeur_id, sec_degradeur_id, treatment_type, comments "
                  "FROM Treatment WHERE dossier = :dossier");
    query.bindValue(":dossier", dossier);
    if (!query.exec()) {
        qWarning() << "PTRepo::GetTreatment Query failed: " << query.lastError();
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    if (!query.next()) {
        qWarning() << "PTRepo::GetTreatment No data found for dossier = " << dossier;
        throw std::runtime_error("Db query returned no data");
    }

    auto patient = this->GetPatientById(query.value(1).toInt());
    auto seances = this->GetSeances(dossier);

    // todo execute lambdas directly instead of first defining them
    auto GetPatientDosimetryWrapper = [&] () -> Dosimetry { try {return GetPatientDosimetry(dossier);}
                                                            catch(...) {} return Dosimetry(); };
    auto patientDosimetry = GetPatientDosimetryWrapper();
    auto GetTopDeJourDosimetryWrapper = [&] (const QDate& date) -> Dosimetry { try {return GetTopDeJourDosimetry(date);}
                                                                               catch(...) {} return Dosimetry(); };
    auto GetReferenceDosimetryWrapper = [&] (const QDateTime& timestamp) -> Dosimetry { try {return GetReferenceDosimetry(timestamp);}
                                                                                        catch(...) {} return Dosimetry(); };

    auto patientRefDosimetry = GetReferenceDosimetryWrapper(patientDosimetry.GetTimestamp());
    auto refDosimetryToday = GetTopDeJourDosimetryWrapper(QDate::currentDate());
    auto dref = GetDefaults().GetDRef();

    auto modulateur(GetModulateurForDossier(dossier));
    auto degradeur_set(GetDegradeurSetForDossier(dossier));

    return Treatment(dossier, patient,
                     query.value(2).toInt(), // active_seance
                     seances,
                     patientDosimetry, patientRefDosimetry, refDosimetryToday, dref,
                     modulateur, degradeur_set, query.value(6).toString(), query.value(7).toString());

}

void PTRepo::UpdateTreatment(const Treatment& treatment) {
    QSqlQuery query(*db_);
    query.prepare("UPDATE Treatment SET active_seance = :active_seance"
                  " WHERE dossier = :dossier");
    query.bindValue(":active_seance", treatment.GetActiveSeanceNumber());
    query.bindValue(":dossier", treatment.GetDossier());

    if (!query.exec()) {
        qWarning() << "PTRepo::UpdateTreatment Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }
}

void PTRepo::UpdateTreatmentType(int dossier, QString type) {
    QSqlQuery query(*db_);

    query.prepare("UPDATE Treatment SET treatment_type = :treatment_type"
                  " WHERE dossier = :dossier"
                  " RETURNING dossier");
    query.bindValue(":treatment_type", type);
    query.bindValue(":dossier", dossier);

    if (!query.exec()) {
        qWarning() << "PTRepo::UpdateTreatmentType Query failed: " << query.lastError();
        throw std::runtime_error("Failed updating treatment type");
    }

    QStringList changed_dossiers;
    while (query.next()) {
        changed_dossiers << query.value(0).toString();
    }

    if (changed_dossiers.empty()) {
        throw std::runtime_error("No dossier was updated");
    }
    if (changed_dossiers.size() > 1) {
        qWarning() << "PTRepo::UpdateTreatmentType Wrong number of dossier updated: " << changed_dossiers.join(", ");
        throw std::runtime_error("A serious problem occurred. More than one dossier was updated.");
    }
}

void PTRepo::SetDossierComments(int dossier, QString comments) {
    QSqlQuery query(*db_);
    query.prepare("UPDATE Treatment SET comments = :comments"
                  " WHERE dossier = :dossier");
    query.bindValue(":comments", comments);
    query.bindValue(":dossier", dossier);

    if (!query.exec()) {
        qWarning() << "PTRepo::SetDossierComments Query failed: " << query.lastError();
        throw std::runtime_error("Failed saving comments to dossier");
    }
}

QString PTRepo::GetDossierComments(int dossier) {
    QSqlQuery query(*db_);
    query.prepare("SELECT comments FROM Treatment WHERE dossier = :dossier");
    query.bindValue(":dossier", dossier);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetDossierComments Query failed: " << query.lastError();
        throw std::runtime_error("Could not get dossier comments");
    }

    if (!query.next()) {
        qWarning() << "PTRepo::GetDossierComments No active treatment";
        throw std::runtime_error("Db returned no data");
    }

    return query.value(0).toString();
}


int PTRepo::GetActiveTreatment() const {
    QSqlQuery query(*db_);
    query.prepare("SELECT dossier FROM ActiveTreatment "
                  "ORDER BY active_treatment_id DESC LIMIT 1");

    if (!query.exec()) {
        qWarning() << "PTRepo::GetActiveTreatment Query failed: " << query.lastError();
        throw std::runtime_error("Could not get active treatment from db");
    }

    if (query.size() != 1) {
        qWarning() << "PTRepo::GetActiveTreatment Did not return a single row: " << query.size();
        throw std::runtime_error("Db returned wrong number of rows");
    }

    if (!query.next()) {
        qWarning() << "PTRepo::GetActiveTreatment No active treatment";
        throw std::runtime_error("Db returned no data");
    }

    return query.value(0).toInt();
}

bool PTRepo::IsActiveTreatmentStarted() const {
    QSqlQuery query(*db_);
    query.prepare("SELECT started FROM ActiveTreatment "
                  "ORDER BY active_treatment_id DESC LIMIT 1");

    if (!query.exec()) {
        qWarning() << "PTRepo::IsActiveTreatmentStarted Query failed: " << query.lastError();
        throw std::runtime_error("Could not get active treatment status from db");
    }

    if (query.size() != 1) {
        qWarning() << "PTRepo::IsActiveTreatmentStarted Did not return a single row: " << query.size();
        throw std::runtime_error("No active treatment found in db");
    }

    if (!query.next()) {
        qWarning() << "PTRepo::IsActiveTreatmentStarted No active treatment";
        throw std::runtime_error("No active treatment found in db");
    }

    return query.value(0).toBool();
}


void PTRepo::SetActiveTreatment(int dossier) {
    QSqlQuery query(*db_);
    query.prepare("INSERT INTO ActiveTreatment(dossier, timestamp, started)"
                  " VALUES(:dossier,:timestamp, :started)");
    query.bindValue(":dossier", dossier);
    query.bindValue(":timestamp", QDateTime::currentDateTime());
    query.bindValue(":started", false);

    if (!query.exec()) {
        qWarning() << "PTRepo::SetActiveTreatment Query failed: " << query.lastError();
        throw std::runtime_error(query.lastError().text().toStdString());
    }
}

void PTRepo::SetActiveTreatmentStarted(bool started) {
    QSqlQuery query(*db_);
    query.prepare("UPDATE ActiveTreatment SET started = :started "
                  " WHERE active_treatment_id IN(SELECT max(active_treatment_id) FROM ActiveTreatment)");
    query.bindValue(":started", started);

    if (!query.exec()) {
        qWarning() << "PTRepo::SetActiveTreatmentStarted Query failed: " << query.lastError();
        throw std::runtime_error(query.lastError().text().toStdString());
    }
}

QString PTRepo::GetTreatmentType(int dossier) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT treatment_type from Treatment where dossier = :dossier");
    query.bindValue(":dossier", dossier);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetTreatmentType Query failed: " << query.lastError();
        throw std::runtime_error("Failed getting treatment type from db");
    }

    if (query.size() != 1) {
        qWarning() << "PTRepo::GetTreatmentType Did not return a single row: " << query.size();
        throw std::runtime_error("Db query returned false number of rows");
    }

    if (!query.next()) {
        qWarning() << "PTRepo::GetTreatmentType Dossier not found";
        throw std::runtime_error("Dossier not found in db");
    }

    return query.value(0).toString();
}

std::vector<TreatmentType> PTRepo::GetTreatmentTypes() const {
    QSqlQuery query(*db_);
    query.prepare("SELECT name, dose_seance_std, dose_seance_boost "
                  "FROM TreatmentType");

    if (!query.exec()) {
        qWarning() << "PTRepo::GetTreatmentTypes Query failed: " << query.lastError();
        throw std::runtime_error("Failed getting treatment types from db");
    }

    std::vector<TreatmentType> types;
    while (query.next()) {
        types.push_back(TreatmentType(query.value(0).toString(), // name
                        dbutil::DecodeDoubleArray(query.value(1).toString()), // dose_seance_std
                        dbutil::DecodeDoubleArray(query.value(2).toString()))); // dose_seance_boost
    }
    return types;
}

void PTRepo::ChangeDegradeurSet(int dossier, int pri_degradeur_id, int sec_degradeur_id) {
    QSqlQuery query(*db_);
        query.prepare("UPDATE Treatment SET pri_degradeur_id = :pri_degradeur_id, sec_degradeur_id = :sec_degradeur_id"
                      " WHERE dossier = :dossier"
                      " RETURNING dossier");
    if (sec_degradeur_id < 0) {
        query.bindValue(":sec_degradeur_id", QVariant(QVariant::Int));
    } else {
        query.bindValue(":sec_degradeur_id", sec_degradeur_id);
    }
    query.bindValue(":pri_degradeur_id", pri_degradeur_id);
    query.bindValue(":dossier", dossier);

    if (!query.exec()) {
        qWarning() << "PTRepo::ChangeDegradeurSet Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    QStringList changed_dossiers;
    while (query.next()) {
        changed_dossiers << query.value(0).toString();
    }

    if (changed_dossiers.empty()) {
        throw std::runtime_error("No dossier was updated");
    }
    if (changed_dossiers.size() > 1) {
        qWarning() << "PTRepo::ChangeDegradeurSet Wrong number of dossier updated: " << changed_dossiers.join(", ");
        throw std::runtime_error("More than one dossier was updated");
    }
}

void PTRepo::ChangeModulateur(const int& dossier, const int& modulateur_id) {
    QSqlQuery query(*db_);

    query.prepare("UPDATE Treatment SET modulateur_id = :modulateur_id"
                  " WHERE dossier = :dossier"
                  " RETURNING dossier");
    query.bindValue(":modulateur_id", modulateur_id);
    query.bindValue(":dossier", dossier);

    if (!query.exec()) {
        qWarning() << "PTRepo::ChangeModulateur Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    QStringList changed_dossiers;
    while (query.next()) {
        changed_dossiers << query.value(0).toString();
    }

    if (changed_dossiers.empty()) {
        throw std::runtime_error("No dossier was updated");
    }
    if (changed_dossiers.size() > 1) {
        qWarning() << "PTRepo::ChangeModulator Wrong number of dossier updated: " << changed_dossiers.join(", ");
        throw std::runtime_error("More than one dossier was updated");
    }
}


bool PTRepo::DosimetryExist(const int& dossier) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT dosimetry_id FROM Dosimetry WHERE dossier = :dossier");
    query.bindValue(":dossier", dossier);

    if (!query.exec()) {
        qWarning() << "Query for patient dosimetry failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    return query.next();
}

Dosimetry PTRepo::GetMeasuredDosimetry(const int& dossier) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT timestamp"
                  " FROM Dosimetry WHERE dossier = :dossier AND monte_carlo = FALSE"
                  " ORDER BY timestamp DESC LIMIT 1");
    query.bindValue(":dossier", dossier);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetMeasuredDosimetry Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    if (!query.next()) {
        qWarning() << "PTRepo::GetMeasuredDosimetry No dosimetry found for dossier = " << dossier;
        throw std::runtime_error("Db query returned no data");
    }

    return GetDosimetry(query.value(0).toDateTime());
}

Dosimetry PTRepo::GetLastMeasuredPatientDosimetry() const {
    QSqlQuery query(*db_);
    query.prepare("SELECT timestamp"
                  " FROM Dosimetry WHERE monte_carlo = FALSE AND reference = FALSE"
                  " ORDER BY timestamp DESC LIMIT 1");

    if (!query.exec()) {
        qWarning() << "PTRepo::GetLastMeasuredDosimetry Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    if (!query.next()) {
        qWarning() << "PTRepo::GetLastMeasuredDosimetry No dosimetry found";
        throw std::runtime_error("Db query returned no data");
    }

    return GetDosimetry(query.value(0).toDateTime());
}

Dosimetry PTRepo::GetMCDosimetry(const int& dossier) {
    QSqlQuery query(*db_);
    query.prepare("SELECT timestamp"
                  " FROM Dosimetry WHERE dossier = :dossier AND monte_carlo = TRUE"
                  " ORDER BY timestamp DESC LIMIT 1");
    query.bindValue(":dossier", dossier);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetMCDosimetry Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    if (!query.next()) {
        qWarning() << "PTRepo::GetMCDosimetry No dosimetry found for dossier = " << dossier;
        throw std::runtime_error("Db query returned no data");
    }

    return GetDosimetry(query.value(0).toDateTime());
}

Dosimetry PTRepo::GetPatientDosimetry(const int& dossier) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT timestamp"
                   " FROM Dosimetry WHERE dossier = :dossier"
                   " ORDER BY timestamp ASC"); // "ASC" since query.last is fetched below (PFFIIU). Change to DESC LIMIT 1 at some point
    query.bindValue(":dossier", dossier);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetPatientDosimetry Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    if (!query.last()) {
        qWarning() << "PTRepo::GetPatientDosimetry No dosimetry found for dossier = " << dossier;
        throw std::runtime_error("Db query returned no data");
    } else {
        qDebug() << "Number of patient dosimetries found: " << query.size();
    }

    return GetDosimetry(query.value(0).toDateTime());
}

Dosimetry PTRepo::GetReferenceDosimetry(const QDateTime& timestamp) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT timestamp FROM Dosimetry "
                  "WHERE Date(timestamp) = :date AND timestamp < :timestamp AND reference = :reference "
                  "ORDER BY timestamp DESC LIMIT 1");
    query.bindValue(":date", timestamp.date());
    query.bindValue(":timestamp", timestamp);
    query.bindValue(":reference", true);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetReferenceDosimetry Query for earlier dosimetry failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    if (query.next()) {
        qDebug() << "PTRepo::GetReferenceDosimetry found dosimetry done before patient dosimetry";
        return GetDosimetry(query.value(0).toDateTime());
    }

    query.clear();
    query.prepare("SELECT timestamp FROM Dosimetry "
                  "WHERE Date(timestamp) = :date AND timestamp > :timestamp AND reference = :reference "
                  "ORDER BY timestamp ASC LIMIT 1");
    query.bindValue(":date", timestamp.date());
    query.bindValue(":timestamp", timestamp);
    query.bindValue(":reference", true);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetReferenceDosimetry Query for later dosimetry failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    if (!query.next()) {
        qWarning() << "PTRepo::GetReferenceDosimetry No dosimetry found after time = " << timestamp.date();
        throw std::runtime_error("Db query returned no data");
    }

    return GetDosimetry(query.value(0).toDateTime());
}

Dosimetry PTRepo::GetTopDeJourDosimetry(const QDate& date) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT timestamp"
                  " FROM Dosimetry WHERE Date(timestamp) = :date AND reference = :reference"
                  " ORDER BY timestamp ASC");
    query.bindValue(":date", date);
    query.bindValue(":reference", true);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetTopDeJourDosimetry Query failed: " << query.lastError();
        throw std::runtime_error("Failed fetching top de jour from db");
    }

    if (!query.last()) {
        qWarning() << "PTRepo::GetTopDeJourDosimetry No dosimetry found for date = " << date;
        throw std::runtime_error("No top de jour found");
    }

    return GetDosimetry(query.value(0).toDateTime());
}

int PTRepo::GetDossierForDosimetry(const QDateTime& timestamp) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT dossier FROM Dosimetry WHERE timestamp = :timestamp");
    query.bindValue(":timestamp", timestamp);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetDossierForDosimetry Query failed: " << query.lastError();
        throw std::runtime_error("Did not find dosimetry");
    }

    if (query.size() != 1) {
        qWarning() << "DbRepositpry::GetDossierForDosimetry Wrong number of dosimetries returned, nmb = " << query.size();
        throw std::runtime_error("A unique dosimetry with the given timestamp was not found");
    }

    if (!query.last()) {
        qWarning() << "PTRepo::GetDossierForDosimetry No dosimetry found with timestamp = " << timestamp;
        throw std::runtime_error("Did not find dosimetry");
    }

    return query.value(0).toInt();
}

Dosimetry PTRepo::GetDosimetry(const QDateTime &timestamp) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT dosimetry_id, timestamp, reference, monte_carlo, temperature, pressure, chambre_id, comment"
                  " FROM Dosimetry WHERE timestamp = :timestamp"
                  " ORDER BY timestamp ASC");
    query.bindValue(":timestamp", timestamp);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetDosimetry Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    if (query.size() != 1) {
        qWarning() << "DbRepositpry::GetDosimetry Wrong number of dosimetries returned, nmb = " << query.size();
        throw std::runtime_error("A unique dosimetry with the given timestamp was not found");
    }

    if (!query.last()) {
        qWarning() << "PTRepo::GetDosimetry No dosimetry found for timestamp = " << timestamp;
        throw std::runtime_error("Db query returned no data");
    }

    std::list<DosimetryRecord> records = this->GetDosimetryRecords(query.value(0).toInt());

    Chambre chambre = this->GetChambre(query.value(6).toInt());
    qDebug() << "Dosimetry done: " << query.value(1).toDateTime().toString("dd.MM.yyyy-hh:mm::ss");

    return Dosimetry(query.value(1).toDateTime(),
                     query.value(2).toBool(),
                     query.value(3).toBool(),
                     query.value(4).toDouble(),
                     query.value(5).toDouble(),
                     chambre,
                     records,
                     query.value(7).toString());
}

std::vector<Debit> PTRepo::GetDebits(const int &dossier, const QDate &from_date, const QDate &to_date) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT debit, debit_stddev, timestamp, monte_carlo, chambre_id FROM DosimetrySummary"
                  " WHERE Dossier = :dossier AND Date(timestamp) >= :from_date AND Date(timestamp) <= :to_date"
                  " ORDER BY timestamp ASC");
    query.bindValue(":dossier", dossier);
    query.bindValue(":from_date", from_date);
    query.bindValue(":to_date", to_date);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetDebitsSinceDate Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    qDebug() << "PTRepo::GetDebitsSinceDate Number of debits found: " << query.size();

    std::vector<Debit> debits;
    while (query.next()) {
        debits.push_back(Debit(query.value(0).toDouble(), // mean
                               query.value(1).toDouble(), // std dev
                               query.value(2).toDateTime(), // timestamp
                               query.value(3).toBool(), // monte carlo
                               this->GetChambre(query.value(4).toInt())));
    }
    return debits;
}

void PTRepo::DeleteSeance(const int &dossier, const int &seance_id) {
    QSqlQuery query(*db_);
    query.prepare("DELETE FROM Seance "
                  " WHERE dossier = :dossier AND seance_id = :seance_id "
                  " RETURNING seance_id");
    query.bindValue(":dossier", dossier);
    query.bindValue(":seance_id", seance_id);

    if (!query.exec()) {
        qWarning() << "PTRepo::DeleteSeance Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    QStringList deleted_seances;
    while (query.next()) {
        deleted_seances << query.value(0).toString();
    }
    if (deleted_seances.empty()) {
        throw std::runtime_error("No seance deleted");
    }
    if (deleted_seances.size() > 1) {
        QString msg("Contact db admin, multiple seances were deleted: " + deleted_seances.join(", "));
        throw std::runtime_error(msg.toStdString());
    }
}

void PTRepo::CreateSeance(const int& dossier, const double& dose_prescribed) {
    QSqlQuery query(*db_);
    query.prepare("INSERT INTO Seance(dossier, dose_prescribed)"
                  " VALUES(:dossier, :dose_prescribed)");
    query.bindValue(":dossier", dossier);
    query.bindValue(":dose_prescribed", dose_prescribed);

    if (!query.exec()) {
        qWarning() << "Query to insert Seance failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }
}

void PTRepo::AddSeanceToExistingTreatment(const int& dossier, const double& dose_prescribed) {
    if (dose_prescribed < 0.0) {
        qWarning() << "PTRepo::AddSeanceToExistingTreatment Negative dose given";
        throw std::runtime_error("A negative dose was given");
    }

    try {
        db_->transaction();

        const int n_seances(GetSeances(dossier).size());
        CreateSeance(dossier, dose_prescribed);

        if (TreatmentFinished(dossier)) {
            QSqlQuery query(*db_);
            query.prepare("UPDATE Treatment SET active_seance = :active_seance"
                          " WHERE dossier = :dossier");
            query.bindValue(":active_seance", n_seances);
            query.bindValue(":dossier", dossier);

            if (!query.exec()) {
                qWarning() << "PTRepo::AddSeanceToExistingTreatment Query failed: " << query.lastError();
                throw std::runtime_error("Db query failed");
            }
        }
        if (!db_->commit()) {
            qWarning() << "PTRepo::AddSeanceToExistingTreatment Commit failed";
            throw std::runtime_error("Db query failed");
        }
    }
    catch (std::exception& exc) {
        qWarning() << "PTRepo::AddSeanceToExistingTreatment Rolling back transaction";
        db_->rollback();
        throw;
    }

}

bool PTRepo::TreatmentFinished(const int& dossier) {
    QSqlQuery query(*db_);
    query.prepare("SELECT active_seance FROM Treatment "
                  " WHERE dossier = :dossier");
    query.bindValue(":dossier", dossier);

    if (!query.exec()) {
        qWarning() << "PTRepo::TreatmentFinished Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    if (!query.next()) {
        qWarning() << "PTRepo::TreatmentFinished No data returned";
        throw std::runtime_error("Db query returned no data");
    }

    return (query.value(0).toInt() == -1);
}

std::vector<Seance> PTRepo::GetSeances(const int& dossier) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT seance_id, dose_prescribed"
                  " FROM Seance WHERE dossier = :dossier"
                  " ORDER BY seance_id ASC");
    query.bindValue(":dossier", dossier);

    if (!query.exec()) {
        qWarning() << "Query for Seances failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    std::vector<Seance> seances;
    while (query.next()) {
        int seance_id(query.value(0).toInt());
        auto records = this->GetSeanceRecords(seance_id);
        seances.push_back(Seance(seance_id,
                                 query.value(1).toDouble(), // dose_prescribed
                                 records));
    }

    return seances;
}

int PTRepo::GetActiveSeanceNumber(int dossier) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT active_seance FROM Treatment WHERE dossier = :dossier");
    query.bindValue(":dossier", dossier);
    if (!query.exec()) {
        qWarning() << "PTRepo::GetActiveSeance Query failed: " << query.lastError();
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    if (query.size() != 1) {
        qWarning() << "PTRepo::GetActiveSeance Wrong number of rows returned " << query.size();
        throw std::runtime_error("A single dossier was not found");
    }

    if (!query.next()) {
        qWarning() << "PTRepo::GetActiveSeance No data found for dossier = " << dossier;
        throw std::runtime_error("No active seance found for dossier");
    }

    return query.value(0).toInt();
}

Seance PTRepo::GetActiveSeance(int dossier) const {
    std::vector<Seance> seances = GetSeances(dossier);
    const int active_seance_number = GetActiveSeanceNumber(dossier);

    if (active_seance_number == -1) {
        qWarning() << "PTRepo::GetActiveSeance Treatment is finished";
        throw std::runtime_error("No active seance in treatment");
    }

    if (active_seance_number >= static_cast<int>(seances.size())) {
        qWarning() << "PTRepo::GetActiveSeance Seance id = " << active_seance_number << " n seances " << seances.size();
        throw std::runtime_error("Non valid active seance number");
    }
    return seances.at(active_seance_number);
}

Seance PTRepo::GetSeance(int seance_id) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT seance_id, dose_prescribed"
                  " FROM Seance WHERE seance_id = :seance_id");
    query.bindValue(":seance_id", seance_id);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetSeance Query for seance failed: " << query.lastError();
        throw std::runtime_error("Failed retrieving seance from db");
    }

    if (query.size() != 1) {
        qWarning() << "PTRepo::GetSeance Wrong number of rows returned " << query.size();
        throw std::runtime_error("A single seance was not found");
    }

    if (!query.next()) {
        qWarning() << "PTRepo::GetSeance No data found for seance_id = " << seance_id;
        throw std::runtime_error("No seance found with the given id");
    }

    auto records = this->GetSeanceRecords(seance_id);
    return Seance(query.value(0).toInt(), // seance_id
                  query.value(1).toDouble(), // dose_prescribed
                  records);
}

void PTRepo::UpdateEstimatedDose(int seance_record_id, double dose) {
    if (dose < 0.0) {
        throw std::runtime_error("The given dose is negative");
    }

    QSqlQuery query(*db_);
    query.prepare("UPDATE SeanceRecord SET est_dose = :est_dose "
                  "WHERE seance_record_id = :seance_record_id");
    query.bindValue(":est_dose", dose);
    query.bindValue(":seance_record_id", seance_record_id);

    if (!query.exec()) {
        qWarning() << "PTRepo::UpdateEstimatedDose Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    if (query.numRowsAffected() != 1) {
        qWarning() << "PTRepo::UpdateEstimatedDose Wrong number of entries updated: " << query.numRowsAffected();
        throw std::runtime_error("Wrong number of entries updated in the database");
    }
}


void PTRepo::UpdateDeliveredDose(const QDateTime& seancerecord_timestamp, const int& um_del) {
    if (um_del < 0) {
        throw std::runtime_error("The given dose is negative");
    }

    try {
        GetSeanceRecord(seancerecord_timestamp);
    }
    catch (std::exception& exc) {
        qWarning() << "PTRepo::UpdateDeliveredDose A unique seancerecord with the given timestamp was not found";
        throw;
    }

    std::vector<int> um_del_vec { um_del };

    QSqlQuery query(*db_);
    query.prepare("UPDATE SeanceRecord SET um_del_1 = :um_del"
                  " WHERE timestamp = :timestamp");
    query.bindValue(":um_del", dbutil::EncodeIntArray(um_del_vec));
    query.bindValue(":timestamp", seancerecord_timestamp);

    if (!query.exec()) {
        qWarning() << "PTRepo::UpdateDeliveredDose Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }
}

void PTRepo::UpdatePlannedDose(const int& seance_id, const int& dossier, const double& dose_gy) {
    if (dose_gy <= 0.0) {
        throw std::runtime_error("The given dose must be positive");
    }

    QSqlQuery query(*db_);
    query.prepare("UPDATE Seance SET dose_prescribed = :dose_prescribed"
                  " WHERE seance_id = :seance_id AND dossier = :dossier"
                  " RETURNING seance_id");
    query.bindValue(":dose_prescribed", dose_gy);
    query.bindValue(":seance_id", seance_id);
    query.bindValue(":dossier", dossier);

    if (!query.exec()) {
        qWarning() << "PTRepo::UpdatePlannedDose Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    QStringList updated_seances;
    while (query.next()) {
        updated_seances << query.value(0).toString();
    }
    if (updated_seances.empty()) {
        throw std::runtime_error("No seance updated");
    }
    if (updated_seances.size() > 1) {
        QString msg("Contact database admin, multiple seances were updated: " + updated_seances.join(", "));
        throw std::runtime_error(msg.toStdString());
    }
}

void PTRepo::SaveSeanceRecord(const int& seance_id, const SeanceRecord& record) {
    QSqlQuery query(*db_);
    query.prepare("INSERT INTO SeanceRecord(seance_id, timestamp, debit, duration,"
                  "i_chambre1, i_chambre2, cf9_status, i_stripper, i_cf9, um_prevu, um_corr, um_del_1, um_del_2, est_dose) "
                  "VALUES(:seance_id, :timestamp, :debit, :duration, :i_chambre1, :i_chambre2, :cf9_status, "
                  ":i_stripper, :i_cf9, :um_prevu, :um_corr, :um_del_1, :um_del_2, :est_dose)");
    query.bindValue(":seance_id", seance_id);
    query.bindValue(":timestamp", record.GetTimestamp());
    query.bindValue(":debit", record.GetDebit());
    query.bindValue(":duration", dbutil::EncodeDoubleArray(record.GetDuration()));
    query.bindValue(":i_chambre1", dbutil::EncodeDoubleArray(record.GetIChambre1()));
    query.bindValue(":i_chambre2", dbutil::EncodeDoubleArray(record.GetIChambre2()));
    query.bindValue(":cf9_status", dbutil::EncodeIntArray(record.GetCF9Status()));
    query.bindValue(":i_stripper", record.GetIStripper());
    query.bindValue(":i_cf9", record.GetICF9());
    query.bindValue(":um_prevu", record.GetUMPrevu());
    query.bindValue(":um_corr", record.GetUMCorr()),
    query.bindValue(":um_del_1", dbutil::EncodeIntArray(record.GetUM1Delivered()));
    query.bindValue(":um_del_2", dbutil::EncodeIntArray(record.GetUM2Delivered()));
    query.bindValue(":est_dose", record.GetDoseEstimated());

    if (!query.exec()) {
        qWarning() << "Query to insert SeanceRecord failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }
}

std::list<SeanceRecord> PTRepo::GetSeanceRecords(const int &seance_id) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT seance_record_id FROM SeanceRecord "
                  "WHERE seance_id = :seance_id "
                  "ORDER BY seance_record_id ASC");
    query.bindValue(":seance_id", seance_id);

    if (!query.exec()) {
        qWarning() << "Query for SeanceRecords failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    std::list<SeanceRecord> seance_records;
    while (query.next()) {
        seance_records.push_back(GetSeanceRecord(query.value(0).toInt()));
    }

    return seance_records;
}

SeanceRecord PTRepo::GetSeanceRecord(const QDateTime &timestamp) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT seance_record_id "
                  "FROM SeanceRecord WHERE timestamp = :timestamp");
    query.bindValue(":timestamp", timestamp);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetSeanceRecord Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    if (static_cast<int>(query.size()) != 1) {
        qWarning() << "PTRepo::GetSeanceRecord Query returned wrong number of rows: " << query.size();
        throw std::runtime_error(((int)query.size() < 1) ? "There is no seance with the given timestamp" : "There are multiple seances with the given timestamp");
    }

    if (!query.next()) {
        throw std::runtime_error("Query::next failed");
    }

    return GetSeanceRecord(query.value(0).toInt());
}


SeanceRecord PTRepo::GetSeanceRecord(const int& seance_record_id) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT timestamp, debit, duration, i_chambre1, i_chambre2, cf9_status, i_stripper, "
                  "i_cf9, um_prevu, um_corr, um_del_1, um_del_2, est_dose "
                  "FROM SeanceRecord WHERE seance_record_id = :seance_record_id");
    query.bindValue(":seance_record_id", seance_record_id);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetSeanceRecord Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    if (static_cast<int>(query.size()) != 1) {
        qWarning() << "PTRepo::GetSeanceRecord Query returned wrong number of rows: " << query.size();
        throw std::runtime_error("There is no seance with the given record id");
    }

    if (!query.next()) {
        throw std::runtime_error("PTRepo::GetSeanceRecord Query::next failed");
    }

    return SeanceRecord(query.value(0).toDateTime(), // timestamp
                        query.value(1).toDouble(), // debit
                        dbutil::DecodeDoubleArray(query.value(2).toString()), // duration
                        dbutil::DecodeDoubleArray(query.value(3).toString()), // i_chambre1
                        dbutil::DecodeDoubleArray(query.value(4).toString()), // i_chambre2
                        dbutil::DecodeIntArray(query.value(5).toString()), // cf9_status
                        query.value(6).toDouble(), // i_stripper
                        query.value(7).toDouble(), // i_cf9
                        query.value(8).toInt(), // um_prevu
                        query.value(9).toInt(), // um_corr
                        dbutil::DecodeIntArray(query.value(10).toString()), // um_del_1
                        dbutil::DecodeIntArray(query.value(11).toString()), // um_del_2
                        query.value(12).toDouble()); // est dose
}

SeanceRecord PTRepo::GetLastSeanceRecord(int dossier) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT seance_record_id FROM SeanceRecord "
                  "WHERE seance_id IN (SELECT seance_id FROM seance WHERE dossier = :dossier) "
                  "ORDER BY seance_record_id DESC LIMIT 1");
    query.bindValue(":dossier", dossier);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetLastSeanceRecord Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    if (query.size() != 1) {
        qWarning() << "PTRepo::GetLastSeanceRecord Query returned wrong number of rows: " << query.size();
        throw std::runtime_error(((int)query.size() < 1) ? "No seance in db for the given dossier" : "Multiple seances in db for the given dossier");
    }

    if (!query.next()) {
        throw std::runtime_error("No seance in db for the given dossier");
    }

    return GetSeanceRecord(query.value(0).toInt());
}

int PTRepo::GetSeanceRecordId(const SeanceRecord& record) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT seance_record_id FROM SeanceRecord "
                  "WHERE timestamp = :timestamp");
    query.bindValue(":timestamp", record.GetTimestamp());

    if (!query.exec()) {
        qWarning() << "PTRepo::GetSeanceRecordId Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    if (query.size() != 1) {
        qWarning() << "PTRepo::GetSeanceRecordId Query returned wrong number of rows: " << query.size();
        throw std::runtime_error(((int)query.size() < 1) ? "No seance record found" : "Multiple seance record returned");
    }

    if (!query.next()) {
        throw std::runtime_error("No seancerecord found with the given dossier number");
    }

    return query.value(0).toInt();
}

void PTRepo::SaveBaliseInstantaneousLevels(const std::vector<BaliseLevel>& levels) {
    for (auto& l : levels) {
        SaveBaliseInstantaneousLevel(l);
    }
}

void PTRepo::SaveBaliseInstantaneousLevel(const BaliseLevel& level) {
    SaveBaliseLevel(QString("BaliseInstantaneousLevel"), level);
}

void PTRepo::SaveBaliseIntegratedLevels(const std::vector<BaliseLevel>& levels) {
    for (auto& l : levels) {
        SaveBaliseIntegratedLevel(l);
    }
}

void PTRepo::SaveBaliseIntegratedLevel(const BaliseLevel& level) {
    SaveBaliseLevel(QString("BaliseIntegratedLevel"), level);
}

void PTRepo::SaveBaliseBufferLevels(const std::vector<BaliseLevel>& levels) {
    for (auto& l : levels) {
        SaveBaliseBufferLevel(l);
    }
}

void PTRepo::SaveBaliseBufferLevel(const BaliseLevel& level) {
    SaveBaliseLevel(QString("BaliseBufferLevel"), level);
}

void PTRepo::SaveBaliseLevel(QString table, const BaliseLevel& level) {
    QSqlQuery query(*db_);
    query.prepare("INSERT INTO " + table + "(timestamp, level)"
                  " VALUES(:timestamp, :level)");
    query.bindValue(":timestamp", level.GetTimestamp());
    query.bindValue(":level", level.GetValue());

    if (!query.exec()) {
        qWarning() << "Query to insert BaliseLevel failed: " << query.lastError();
        throw std::runtime_error("Failed inserting balise level");
    }
}

std::vector<BaliseLevel> PTRepo::GetInstantaneousBaliseLevels(const QDate &from_date, const QDate &to_date) const {
    return GetBaliseLevels(QString("BaliseInstantaneousLevel"), from_date, to_date);
}

std::vector<BaliseLevel> PTRepo::GetIntegratedBaliseLevels(const QDate &from_date, const QDate &to_date) const {
    return GetBaliseLevels(QString("BaliseIntegratedLevel"), from_date, to_date);
}

std::vector<BaliseLevel> PTRepo::GetBufferBaliseLevels(const QDate &from_date, const QDate &to_date) const {
    return GetBaliseLevels(QString("BaliseBufferLevel"), from_date, to_date);
}

std::vector<BaliseLevel> PTRepo::GetBaliseLevels(QString table, const QDate &from_date, const QDate &to_date) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT timestamp, level "
                  "FROM " + table + " WHERE Date(timestamp) >= :from_date AND Date(timestamp) <= :to_date "
                  "ORDER BY TIMESTAMP ASC");
    query.bindValue(":from_date", from_date);
    query.bindValue(":to_date", to_date);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetBaliseLevels Query failed: " << query.lastError();
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    std::vector<BaliseLevel> levels;
    while (query.next()) {
        levels.push_back(BaliseLevel(query.value(0).toDateTime(),
                                       query.value(1).toDouble()));
    }
    return levels;
}

void PTRepo::SaveBaliseRecord(const BaliseRecord& record) {
    QSqlQuery query(*db_);
    query.prepare("INSERT INTO BaliseRecord(timestamp, dossier, seance_record_id, "
                  "charge, ref_debit, tdj_debit, patient_debit, dose_gy)"
                  " VALUES(:timestamp, :dossier, :seance_record_id, :charge, :ref_debit, :tdj_debit,"
                  " :patient_debit, :dose_gy)");
    query.bindValue(":timestamp", record.GetTimestamp());
    query.bindValue(":dossier", record.GetDossierNumber());
    query.bindValue(":seance_record_id", record.GetSeanceRecordId());
    query.bindValue(":charge", record.GetCharge());
    query.bindValue(":ref_debit", record.GetRefDebit());
    query.bindValue(":tdj_debit", record.GetTDJDebit());
    query.bindValue(":patient_debit", record.GetPatientDebit());
    query.bindValue(":dose_gy", record.GetDose());

    if (!query.exec()) {
        qWarning() << "Query to insert BaliseRecord failed: " << query.lastError();
        throw std::runtime_error("Failed inserting balise record");
    }
}

BaliseRecord PTRepo::GetBaliseRecord(const SeanceRecord& record) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT timestamp, dossier, seance_record_id, charge, ref_debit, tdj_debit, patient_debit, dose_gy "
                  "FROM BaliseRecord WHERE seance_record_id = :seance_record_id");
    query.bindValue(":seance_record_id", GetSeanceRecordId(record));

    if (!query.exec()) {
        qWarning() << "GetBaliseRecord failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    if (static_cast<int>(query.size()) != 1) {
        qWarning() << "PTRepo::GetBaliseRecord Query returned wrong number of rows: " << query.size();
        throw std::runtime_error(((int)query.size() < 1) ? "There is no baliserecord with the given seance record id" : "There are multiple balise record for the given seance record id");
    }

    if (!query.next()) {
        throw std::runtime_error("Query::next failed");
    }

    return BaliseRecord(query.value(0).toDateTime(),
                        query.value(1).toInt(), // dossier
                        query.value(2).toInt(), // seance record id
                        query.value(3).toDouble(), // charge
                        query.value(4).toDouble(), // ref debit
                        query.value(5).toDouble(), // tdj debit
                        query.value(6).toDouble(), // patient debit
                        query.value(7).toDouble()); // dose
}

std::vector<BaliseRecord> PTRepo::GetBaliseRecords(const QDate& from_date, const QDate& to_date) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT timestamp, dossier, seance_record_id, charge, ref_debit, tdj_debit, patient_debit, dose_gy "
                  "FROM BaliseRecord WHERE Date(timestamp) >= :from_date AND Date(timestamp) <= :to_date "
                  "ORDER BY TIMESTAMP ASC");
    query.bindValue(":from_date", from_date);
    query.bindValue(":to_date", to_date);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetBaliseRecords Query failed: " << query.lastError();
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    std::vector<BaliseRecord> records;
    while (query.next()) {
        records.push_back(BaliseRecord(query.value(0).toDateTime(),
                                       query.value(1).toInt(), // dossier
                                       query.value(2).toInt(), // seance record id
                                       query.value(3).toDouble(), // charge
                                       query.value(4).toDouble(), // ref debit
                                       query.value(5).toDouble(), // tdj debit
                                       query.value(6).toDouble(), // patient debit
                                       query.value(7).toDouble())); // dose gy
    }
    return records;
}

BaliseCalibration PTRepo::GetBaliseCalibration() const {
    QSqlQuery query(*db_);
    query.prepare("SELECT timestamp, sv_per_coulomb, doseRateToH_alpha, doseRateToH_beta, HToDoseRate_alpha, HToDoseRate_beta "
                  "FROM BaliseCalibration "
                  "ORDER BY timestamp DESC LIMIT 1");

    if (!query.exec()) {
        qWarning() << "GetBaliseCalibration failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    if (static_cast<int>(query.size()) != 1) {
        qWarning() << "PTRepo::GetBaliseCalibration Query returned wrong number of rows: " << query.size();
        throw std::runtime_error(((int)query.size() < 1) ? "There is no balisecalibration" : "Multiple balise calibrations were returned");
    }

    if (!query.next()) {
        throw std::runtime_error("Query::next failed");
    }

    return BaliseCalibration(query.value(0).toDateTime(), // timestamp
                             query.value(1).toDouble(), // sv_per_coulomb
                             query.value(2).toDouble(), // doseratetoh_alpha
                             query.value(3).toDouble(), // doseratetoh_beta
                             query.value(4).toDouble(), // htodoserate_alpha
                             query.value(5).toDouble()); // htodoserate_beta
}

BaliseConfiguration PTRepo::GetBaliseConfiguration() const {
    QSqlQuery query(*db_);
    query.prepare("SELECT timestamp, mode_functionnement, preampli_type, preampli_gamme, instantaneous_lam, "
                  "integrated1_lam, integrated2_lam, command_confirmation, include_date_in_measurement, "
                  "include_raw_in_measurement, instantaneous_time, instantaneous_elements, integrated1_time, "
                  "integrated2_time, number_of_measurements, instantaneous_conversion_coefficient, integrated_conversion_coefficient, "
                  "integrated_threshold, threshold_A, threshold_B, threshold_C "
                  "FROM BaliseConfiguration "
                  "ORDER BY timestamp DESC LIMIT 1");

    if (!query.exec()) {
        qWarning() << "GetBaliseConfiguration failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    if (static_cast<int>(query.size()) != 1) {
        qWarning() << "PTRepo::GetBaliseConfiguration Query returned wrong number of rows: " << query.size();
        throw std::runtime_error(((int)query.size() < 1) ? "There is no balise configuration" : "Multiple balise configurations were returned");
    }

    if (!query.next()) {
        throw std::runtime_error("Query::next failed");
    }

    return BaliseConfiguration(query.value(0).toDateTime(), // timestamp
                               query.value(1).toInt(), // mode functionnement
                               query.value(2).toInt(), // preamplitype
                               query.value(3).toInt(), // preampligamme
                               query.value(4).toBool(), // instantaneous lam
                               query.value(5).toBool(), // integrated1 lam
                               query.value(6).toBool(), // integrated2 lam
                               query.value(7).toBool(), // command confirmation
                               query.value(8).toBool(), // include date in measurement
                               query.value(9).toBool(), // include raw in measurement
                               query.value(10).toInt(), // instantaneous time
                               query.value(11).toInt(), // instantaneous elements
                               query.value(12).toInt(), // integrated1 time
                               query.value(13).toInt(), // integrated2 time
                               query.value(14).toInt(), // number of measurements
                               query.value(15).toDouble(), // instantaneous conversion coefficient
                               query.value(16).toDouble(), // integrated conversion coefficient
                               query.value(17).toDouble(), // integrated threshold
                               query.value(18).toDouble(), // threshold A
                               query.value(19).toDouble(), // threshold B
                               query.value(20).toDouble()); // threshold C
}

void PTRepo::SaveBaliseCalibration(const BaliseCalibration& calib) {
    QSqlQuery query(*db_);
    query.prepare("INSERT INTO BaliseCalibration(timestamp, sv_per_coulomb, doseRateToH_alpha, doseRateToH_beta, HToDoseRate_alpha, HToDoseRate_beta) "
                  "VALUES(:timestamp, :sv_per_coulomb, :doseRateToH_alpha, :doseRateToH_beta, :HToDoseRate_alpha, :HToDoseRate_beta)");
    query.bindValue(":timestamp", calib.timestamp());
    query.bindValue(":sv_per_coulomb", calib.svPerCoulomb());
    query.bindValue(":doseRateToH_alpha", calib.DoseRateToH_alpha());
    query.bindValue(":doseRateToH_beta", calib.DoseRateToH_beta());
    query.bindValue(":HToDoseRate_alpha", calib.HToDoseRate_alpha());
    query.bindValue(":HToDoseRate_beta", calib.HToDoseRate_beta());

    if (!query.exec()) {
        qWarning() << "Query to insert BaliseCalibration failed: " << query.lastError();
        throw std::runtime_error("Failed inserting balise calibration");
    }
}


void PTRepo::SaveXRayRecord(const XRayRecord& record) {
    QSqlQuery query(*db_);
    query.prepare("INSERT INTO XRayRecord(timestamp, dossier, peak_value_sv, integrated_charge)"
                  " VALUES(:timestamp, :dossier, :peak_value_sv, :integrated_charge)");
    query.bindValue(":timestamp", record.GetTimestamp());
    query.bindValue(":dossier", record.GetDossierNumber());
    query.bindValue(":peak_value_sv", record.GetPeakValue());
    query.bindValue(":integrated_charge", record.GetIntegratedCharge());

    if (!query.exec()) {
        qWarning() << "PTRepo::SaveXRayRecord Query to insert XRayRecord failed: " << query.lastError();
        throw std::runtime_error("Failed inserting xray record");
    }
}

std::vector<XRayRecord> PTRepo::GetXRayRecords(int dossier) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT timestamp, dossier, peak_value_sv, integrated_charge "
                  "FROM XRayRecord WHERE dossier = :dossier "
                  "ORDER BY TIMESTAMP ASC");
    query.bindValue(":dossier", dossier);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetXRayRecords Query failed: " << query.lastError();
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    std::vector<XRayRecord> records;
    while (query.next()) {
        records.push_back(XRayRecord(query.value(0).toDateTime(),
                                     query.value(1).toInt(), // dossier
                                     query.value(2).toDouble(), // peak value
                                     query.value(3).toDouble())); // integrated charge
    }
    return records;
}

std::vector<double> PTRepo::GetXRayPeakDoseRate(const QDate& from, const QDate& to) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT peak_value_sv "
                  "FROM XRayRecord "
                  "WHERE Date(timestamp) >= :from_date AND Date(timestamp) <= :to_date "
                  "ORDER BY TIMESTAMP ASC");
    query.bindValue(":from_date", from);
    query.bindValue(":to_date", to);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetXRayRecords Query failed: " << query.lastError();
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    std::vector<double> dose_rate;
    while (query.next()) {
        dose_rate.push_back(1.0E6 * query.value(0).toDouble());
    }
    return dose_rate;
}

std::vector<double> PTRepo::GetXRayIntegratedCharge(const QDate& from, const QDate& to) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT integrated_charge "
                  "FROM XRayRecord "
                  "WHERE Date(timestamp) >= :from_date AND Date(timestamp) <= :to_date "
                  "ORDER BY TIMESTAMP ASC");
    query.bindValue(":from_date", from);
    query.bindValue(":to_date", to);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetXRayIntegratedCharge Query failed: " << query.lastError();
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    std::vector<double> dose_rate;
    while (query.next()) {
        dose_rate.push_back(1.0E12 * query.value(0).toDouble());
    }
    return dose_rate;
}

std::pair<int, double> PTRepo::GetXRayExposure(int dossier) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT COUNT(*), SUM(integrated_charge) FROM XRayRecord WHERE dossier = :dossier and integrated_charge > 0");
    query.bindValue(":dossier", dossier);
    if (!query.exec()) {
        qWarning() << "PTRepo::GetNmbXRayRecords Query failed: " << query.lastError();
        throw std::runtime_error("Failed getting number of xray records from db");
    }
    if (!query.next()) {
        throw std::runtime_error("No result returned");
    }
    return std::make_pair(query.value(0).toInt(),
                          0.25*query.value(1).toDouble()*1.0E12);
}


void PTRepo::SaveDosimetry(const int& dossier, const Dosimetry& dosimetry) {
    try {
        db_->transaction();
        QSqlQuery query(*db_);
        query.prepare("INSERT INTO Dosimetry(dossier, timestamp, reference, monte_carlo, temperature, pressure,"
                      " chambre_id, comment)"
                      " VALUES(:dossier, :timestamp, :reference, :monte_carlo, :temperature, :pressure,"
                      " :chambre_id, :comment)"
                      " RETURNING dosimetry_id");
        query.bindValue(":dossier", dossier);
        query.bindValue(":timestamp",  dosimetry.GetTimestamp());
        query.bindValue(":reference", dosimetry.IsReference());
        query.bindValue(":monte_carlo", dosimetry.IsMonteCarlo());
        query.bindValue(":temperature", dosimetry.GetTemperature());
        query.bindValue(":pressure", dosimetry.GetPressure());
        query.bindValue(":chambre_id", dosimetry.GetChambre().id());
        query.bindValue(":comment", dosimetry.GetComment());

        if (!query.exec()) {
            qWarning() << "Query to save Dosimetry failed: " << query.lastError();
            throw std::runtime_error("Db query failed");
        }

        if (!query.next()) {
            qWarning() << "Query didn't return autoincrement id";
            throw std::runtime_error("Db query failed");
        }

        int lastInsertedId = query.value(0).toInt();
        this->SaveDosimetryRecords(lastInsertedId, dosimetry.GetRecords());

        this->SaveDosimetrySummary(dossier, dosimetry);

        if (!db_->commit()) {
            qWarning() << "Commit failed";
            throw std::runtime_error("Db query failed");
        }
    }
    catch (...) {
        qWarning() << "Rolling back SaveDosimetry insert query";
        db_->rollback();
        throw;
    }
}

void PTRepo::SaveMonteCarloDosimetry(const int &dossier, const double &debit) {
    if (dossier == 10000) {
        throw std::runtime_error("Cannot save dosimetry to top de jour dossier");
    }
    if (debit < 0.001) {
        throw std::runtime_error("Debit is zero or negative");
    }

    auto defaults = GetDefaults();
    Dosimetry mcDosimetry(QDateTime::currentDateTime(),
                          false, // reference
                          true, // monte carlo
                          defaults.GetTemperature(),
                          defaults.GetPressure(),
                          GetChambre(defaults.GetChambre()),
                          std::list<DosimetryRecord>(),
                          ""); // comment
    const int um(1000); // arbitrary
    const double corresponding_charge(mcDosimetry.GetExpectedCharge(debit, um));
    DosimetryRecord record(um, corresponding_charge);
    mcDosimetry.AddRecord(record);
    this->SaveDosimetry(dossier, mcDosimetry);
}

void PTRepo::SaveDosimetrySummary(const int& dossier, const Dosimetry& dosimetry) {
    QSqlQuery query(*db_);
    query.prepare("INSERT INTO DosimetrySummary(dossier, timestamp, reference, monte_carlo, chambre_id, debit, debit_stddev) "
                  " VALUES(:dossier, :timestamp, :reference, :monte_carlo, :chambre_id, :debit, :debit_stddev)");
    query.bindValue(":dossier", dossier);
    query.bindValue(":timestamp", dosimetry.GetTimestamp());
    query.bindValue(":reference", dosimetry.IsReference());
    query.bindValue(":monte_carlo", dosimetry.IsMonteCarlo());
    query.bindValue(":chambre_id", dosimetry.GetChambre().id());
    query.bindValue(":debit", dosimetry.GetDebitMean());
    query.bindValue(":debit_stddev", dosimetry.GetDebitStdDev());

    if (!query.exec()) {
        qWarning() << "PTRepo::SaveDosimetrySummary Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }
}

void PTRepo::SaveDosimetryRecords(const int& dosimetry_id, const std::list<DosimetryRecord>& records) {
    QSqlQuery query(*db_);
    for (DosimetryRecord record : records) {
        query.clear();
        query.prepare("INSERT INTO DosimetryRecord(dosimetry_id, duration, i_chambre1, i_chambre2, i_stripper, i_cf9, um_prevu, um_del, charge, air_ic_charge) "
                      "VALUES(:dosimetry_id, :duration, :i_chambre1, :i_chambre2, :i_stripper, :i_cf9, :um_prevu, :um_del, :charge, :air_ic_charge)");
        query.bindValue(":dosimetry_id", dosimetry_id);
        query.bindValue(":duration", dbutil::EncodeDoubleArray(record.GetDuration()));
        query.bindValue(":i_chambre1", dbutil::EncodeDoubleArray(record.GetIChambre1()));
        query.bindValue(":i_chambre2", dbutil::EncodeDoubleArray(record.GetIChambre2()));
        query.bindValue(":i_stripper", record.GetIStripper());
        query.bindValue(":i_cf9", record.GetICF9());
        query.bindValue(":um_prevu", record.GetUMPrevu());
        query.bindValue(":um_del", dbutil::EncodeIntArray(record.GetUMDelivered()));
        query.bindValue(":charge", record.GetCharge());
        query.bindValue(":air_ic_charge", record.GetAirICCharge());

        if (!query.exec()) {
            qWarning() << "PTRepo::SaveDosimetryRecords Query failed: " << query.lastError();
            throw std::runtime_error("Db query failed");
        }
    }
}

std::list<DosimetryRecord> PTRepo::GetDosimetryRecords(const int& dosimetry_id) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT duration, i_chambre1, i_chambre2, i_stripper, i_cf9, um_prevu, um_del, charge, air_ic_charge "
                  "FROM DosimetryRecord WHERE dosimetry_id = :dosimetry_id");
    query.bindValue(":dosimetry_id", dosimetry_id);

    if (!query.exec()) {
        qWarning() << "Query for DosimetryRecords failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    std::list<DosimetryRecord> dosimetry_records;
    while (query.next()) {
        dosimetry_records.push_back(DosimetryRecord(dbutil::DecodeDoubleArray(query.value(0).toString()),
                                                    dbutil::DecodeDoubleArray(query.value(1).toString()),
                                                    dbutil::DecodeDoubleArray(query.value(2).toString()),
                                                    query.value(3).toDouble(), // stripper
                                                    query.value(4).toDouble(), // cf9
                                                    query.value(5).toInt(), // um_prevu
                                                    dbutil::DecodeIntArray(query.value(6).toString()), // um_del
                                                    query.value(7).toDouble(), // charge
                                                    query.value(8).toDouble())); // air_ic_charge
    }
    return dosimetry_records;
}

Chambre PTRepo::GetChambre(const QString& name) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT chambre_id "
                  "FROM Chambre WHERE name = :name "
                  "ORDER BY timestamp DESC LIMIT 1");
    query.bindValue(":name", name);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetChambre Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    if (!query.next()) {
        qWarning() << "PTRepo::GetChambre No Chambre found with name = " << name;
        throw std::runtime_error("Db query returned no data");
    }
    Chambre chambre = GetChambre(query.value(0).toInt());
    return chambre;
}

Chambre PTRepo::GetChambre(const int& id) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT chambre_id, timestamp, name, nd, kqq0, yoffset, zoffset FROM Chambre "
                  "WHERE chambre_id = :chambre_id");
    query.bindValue(":chambre_id", id);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetChambre Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    if (!query.next()) {
        qWarning() << "PTRepo::GetChambre No Chambre found with id = " << id;
        throw std::runtime_error("Db query returned no data");
    }
    Chambre chambre(query.value(0).toInt(),
                    query.value(1).toDateTime(),
                    query.value(2).toString(),
                    query.value(3).toDouble(),
                    query.value(4).toDouble(),
                    query.value(5).toDouble(),
                    query.value(6).toDouble());
    return chambre;
}


QStringList PTRepo::GetChambres() const {
    QSqlQuery query(*db_);
    query.prepare("SELECT DISTINCT name FROM Chambre");

    if (!query.exec()) {
        qWarning() << "Query for Chambres failed: " << query.lastError();
        throw std::runtime_error("Db query for chambres failed");
    }

    QStringList chambres;
    while (query.next()) {
        chambres.push_back(query.value(0).toString());
    }
    return chambres;
}

void PTRepo::SaveModulateurItems(const Modulateur& modulateur, QString fraiseuse_program, bool update_existing) {
    try {
        db_->transaction();

        if (update_existing) {
            UpdateModulateur(modulateur);
        } else {
            SaveModulateur(modulateur);
        }

        SaveTreatmentFraiseuseProgram(modulateur.id().toInt(), Modulateur::program_type, QDateTime::currentDateTime(), fraiseuse_program);

        if (!db_->commit()) {
            qWarning() << "PTRepo::SaveModulateurItems Commit failed";
            throw std::runtime_error("Db query failed");
        }
    }
    catch (...) {
        qWarning() << "PTRepo::SaveModulateurItems Rolling back...";
        db_->rollback();
        throw;
    }
}

void PTRepo::UpdateModulateur(const Modulateur& modulateur) {
    QSqlQuery query(*db_);
    query.prepare("UPDATE Modulateur SET modulation_100 = :modulation_100, modulation_98 = :modulation_98, parcours = :parcours, um_degradeur = :um_degradeur, "
                  "um_plexi_weight = :um_plexi_weight, available = :available, opt_alg = :opt_alg, data_set = :data_set, n_sectors = :n_sectors, "
                  "mod_step = :mod_step, input_parcours = :input_parcours, input_mod = :input_mod, decalage = :decalage, conv_criteria = :conv_criteria "
                  "WHERE modulateur_id = :modulateur_id "
                  "RETURNING modulateur_id");
    query.bindValue(":modulateur_id", modulateur.id());
    query.bindValue(":modulation_100", 0.0);
    query.bindValue(":modulation_98", 0.0);
    query.bindValue(":parcours", 0.0);
    query.bindValue(":um_degradeur", modulateur.RangeShift());
    query.bindValue(":um_plexi_weight", dbutil::EncodeIntDoubleMap(modulateur.um_plexi_weight()));
    query.bindValue(":available", false);
    query.bindValue(":opt_alg", modulateur.opt_alg());
    query.bindValue(":data_set", modulateur.data_set());
    query.bindValue(":n_sectors", modulateur.n_sectors());
    query.bindValue(":mod_step", modulateur.mod_step());
    query.bindValue(":input_parcours", modulateur.input_parcours());
    query.bindValue(":input_mod", modulateur.input_mod());
    query.bindValue(":decalage", modulateur.decalage());
    query.bindValue(":conv_criteria", modulateur.conv_criteria());

    if (!query.exec()) {
        qWarning() << "Query to update modulateur failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    QStringList updated_modulateurs;
    while (query.next()) {
        updated_modulateurs << query.value(0).toString();
    }

    if (updated_modulateurs.empty()) {
        qWarning() << "PTRepo::UpdateModulateur No modulateur was updated";
        throw std::runtime_error("Update modulateur failed");

    }

    if (updated_modulateurs.size() > 1) {
        qWarning() << "PTRepo::UpdateModulateur Several modulateurs were updated by mistake: " << updated_modulateurs.join(", ");
        throw std::runtime_error("Several modulateurs were updated by mistake: " + updated_modulateurs.join(", ").toStdString());
    }
}

void PTRepo::SaveModulateur(const Modulateur& modulateur) {
    QSqlQuery query(*db_);
    query.prepare("INSERT INTO Modulateur(modulateur_id, modulation_100, modulation_98, parcours, um_degradeur, um_plexi_weight, available, "
                  "opt_alg, data_set, n_sectors, mod_step, input_parcours, input_mod, decalage, conv_criteria) "
                  "VALUES(:modulateur_id, :modulation_100, :modulation_98, :parcours, :um_degradeur, :um_plexi_weight, :available, "
                  ":opt_alg, :data_set, :n_sectors, :mod_step, :input_parcours, :input_mod, :decalage, :conv_criteria)");
    query.bindValue(":modulateur_id", modulateur.id());
    query.bindValue(":modulation_100", 0.0);
    query.bindValue(":modulation_98", 0.0);
    query.bindValue(":parcours", 0.0);
    query.bindValue(":um_degradeur", modulateur.RangeShift());
    query.bindValue(":um_plexi_weight", dbutil::EncodeIntDoubleMap(modulateur.um_plexi_weight()));
    query.bindValue(":available", false);
    query.bindValue(":opt_alg", modulateur.opt_alg());
    query.bindValue(":data_set", modulateur.data_set());
    query.bindValue(":n_sectors", modulateur.n_sectors());
    query.bindValue(":mod_step", modulateur.mod_step());
    query.bindValue(":input_parcours", modulateur.input_parcours());
    query.bindValue(":input_mod", modulateur.input_mod());
    query.bindValue(":decalage", modulateur.decalage());
    query.bindValue(":conv_criteria", modulateur.conv_criteria());

    if (!query.exec()) {
        qWarning() << "PTRepo::SaveModulateur Query failed: " << query.lastError();
        throw std::runtime_error("Saving modulateur to db failed");
    }
}

Modulateur PTRepo::GetModulateurForDossier(const int& dossier) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT modulateur_id FROM Treatment WHERE dossier = :dossier");
    query.bindValue(":dossier", dossier);

    if (!query.exec()) {
        qWarning() << QString("PTRepo::GetModulateurForDossier Query failed: ") << query.lastError();
        throw std::runtime_error("Db query for modulateur failed");
    }

    if (!query.next()) {
        qWarning() << QString("PTRepo::GetModulateurForDossier No data for dossier = ") << dossier;
        throw std::runtime_error("No modulateur for the given dossier found");
    }

    return GetModulateur(query.value(0).toInt());
}

Modulateur PTRepo::GetModulateur(const int& modulateur_id) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT modulateur_id, modulation_100, modulation_98, parcours, um_degradeur, um_plexi_weight, available, "
                  "opt_alg, data_set, n_sectors, mod_step, input_parcours, input_mod, decalage, conv_criteria "
                  "FROM Modulateur "
                  "WHERE modulateur_id = :modulateur_id");
    query.bindValue(":modulateur_id", modulateur_id);

    if (!query.exec()) {
        qWarning() << QString("PTRepo::GetModulateur Query failed: ") << query.lastError();
        throw std::runtime_error("Db query for modulateur failed");
    }

    if (!query.next()) {
        qWarning() << QString("PTRepo::GetModulateur No modulateur found with id = ") << modulateur_id;
        throw std::runtime_error("No modulateur with the given id found");
    }

    return Modulateur(query.value(0).toString(), // id
                       query.value(1).toDouble(), // mod100
                       query.value(2).toDouble(), // mod98
                       query.value(3).toDouble(), // parcours
                       query.value(4).toInt(), // um_degradeur
                       dbutil::DecodeIntDoubleMap(query.value(5).toString()), // weights
                       query.value(6).toBool(), // available
                       query.value(7).toString(), // opt_alg
                      query.value(8).toString(), // data_set
                      query.value(9).toInt(), // n_sectors
                      query.value(10).toDouble(), // mod_step
                      query.value(11).toDouble(), // input_parcours
                      query.value(12).toDouble(), // input mod
                      query.value(13).toDouble(), // decalage
                      query.value(14).toDouble()); // conv criteria
}

bool PTRepo::ModulateurExist(int modulateur_id) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT modulateur_id FROM Modulateur "
                  "WHERE modulateur_id = :modulateur_id");
    query.bindValue(":modulateur_id", modulateur_id);

    if (!query.exec()) {
        qWarning() << QString("PTRepo::ModulateurExist Query failed: ") << query.lastError();
        throw std::runtime_error("Db query for modulateur failed");
    }

    return query.next();
}


std::vector<Modulateur> PTRepo::GetModulateurs(const double& min_mod100, const double& max_mod100,
                                                     const double& min_mod98, const double& max_mod98,
                                                     const double& min_parcours, const double& max_parcours) {
    QSqlQuery query(*db_);
    query.prepare("SELECT modulateur_id FROM Modulateur "
                  "WHERE modulation_100 BETWEEN :min_mod100 AND :max_mod100 AND "
                  "modulation_98 BETWEEN :min_mod98 AND :max_mod98 AND "
                  "parcours BETWEEN :min_parcours AND :max_parcours AND "
                  "available = true");

    query.bindValue(":min_mod100", min_mod100);
    query.bindValue(":max_mod100", max_mod100);
    query.bindValue(":min_mod98", min_mod98);
    query.bindValue(":max_mod98", max_mod98);
    query.bindValue(":min_parcours", min_parcours);
    query.bindValue(":max_parcours", max_parcours);

    if (!query.exec()) {
        qWarning() << QString("PTRepo::GetModulateurs Query failed: ") << query.lastError();
        throw std::runtime_error("Db query for modulateurs failed");
    }

    std::vector<Modulateur> modulateurs;
    while (query.next()) {
        modulateurs.push_back(GetModulateur(query.value(0).toInt()));
    }

    return modulateurs;
}

QStringList PTRepo::GetAvailableModulateurs() const {
    QSqlQuery query(*db_);
    query.prepare("SELECT modulateur_id FROM Modulateur "
                  "WHERE available = true "
                  "ORDER BY modulateur_id ASC");

    if (!query.exec()) {
        qWarning() << "Query for Modulateurs failed: " << query.lastError();
        throw std::runtime_error("Db query for modulateurs failed");
    }

    QStringList modulateurs;
    while (query.next()) {
        modulateurs.push_back(query.value(0).toString());
    }
    return modulateurs;
}

DegradeurSet PTRepo::GetDegradeurSetForDossier(const int& dossier) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT pri_degradeur_id, sec_degradeur_id FROM Treatment"
                  " WHERE dossier = :dossier");
    query.bindValue(":dossier", dossier);

    if (!query.exec()) {
        qWarning() << QString("PTRepo::GetDegradeurSet Query failed: ") << query.lastError();
        throw std::runtime_error("Db query for degradeur failed");
    }

    if (!query.next()) {
        qWarning() << QString("PTRepo::GetDegradeurSet No degradeurs found for dossier = ") << dossier;
        throw std::runtime_error("No degradeur for the given dossier found");
    }

    if (query.value(1).isNull()) {
        return DegradeurSet(GetDegradeur(query.value(0).toInt()));
    } else {
        return DegradeurSet(GetDegradeur(query.value(0).toInt()),
                            GetDegradeur(query.value(1).toInt()));
    }
}

Degradeur PTRepo::GetDegradeur(const int& degradeur_id) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT degradeur_id, um_plexi, available FROM Degradeur WHERE degradeur_id = :degradeur_id");
    query.bindValue(":degradeur_id", degradeur_id);

    if (!query.exec()) {
        qWarning() << QString("PTRepo::GetDegradeur Query failed: ") << query.lastError();
        throw std::runtime_error("Db query for degradeur failed");
    }

    if (!query.next()) {
        qWarning() << QString("PTRepo::GetDegradeur No degradeur found with id = ") << degradeur_id;
        throw std::runtime_error("No degradeur with the given id found");
    }

  return Degradeur(query.value(0).toString(),
                   query.value(1).toInt(),
                   query.value(2).toBool());
}

std::vector<Degradeur> PTRepo::GetAllAvailableDegradeurs() const {
    return this->GetAvailableDegradeurs(100000);
}

std::vector<Degradeur> PTRepo::GetAvailableDegradeurs(const int& max_um_plexi) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT degradeur_id FROM Degradeur "
                  "WHERE um_plexi < :um_plexi AND available = true "
                  "ORDER BY degradeur_id ASC");
    query.bindValue(":um_plexi", max_um_plexi);

    if (!query.exec()) {
        qWarning() << QString("PTRepo::GetDegradeurs Query failed: ") << query.lastError();
        throw std::runtime_error("Db query for degradeurs failed");
    }

    std::vector<Degradeur> degradeurs;
    while (query.next()) {
        degradeurs.push_back(GetDegradeur(query.value(0).toInt()));
    }
    return degradeurs;
}

std::vector<DegradeurSet> PTRepo::GetBestDegradeurCombinations(const int& des_um_plexi, const int& max_um_plexi_diff) const {
    auto degradeurs = this->GetAvailableDegradeurs(des_um_plexi + max_um_plexi_diff);

    std::vector<DegradeurSet> degradeur_sets;
    for (auto it_pri = degradeurs.begin(); it_pri != degradeurs.end(); ++it_pri) {
        // select single degradeur
        if (std::abs((*it_pri).um_plexi() - des_um_plexi) < max_um_plexi_diff) {
            degradeur_sets.push_back(DegradeurSet(*it_pri));
        }
        // select pairs of degradeurs
        for (auto it_sec = std::next(it_pri, 1); it_sec != degradeurs.end(); ++it_sec) {
            if (std::abs((*it_pri).um_plexi() + (*it_sec).um_plexi() - des_um_plexi) < max_um_plexi_diff) {
                degradeur_sets.push_back(DegradeurSet(*it_pri, *it_sec));
            }
        }
    }

    // sort according to match with um_plexi
    auto sortRule = [&] (const DegradeurSet& d1, const DegradeurSet& d2) -> bool
    { return ((((d1.um_plexi_total() - des_um_plexi) == (d2.um_plexi_total() - des_um_plexi)) && d1.single()) ||
              (std::abs(d1.um_plexi_total() - des_um_plexi) < std::abs(d2.um_plexi_total() - des_um_plexi))); };

    std::sort(degradeur_sets.begin(), degradeur_sets.end(), sortRule);
    return degradeur_sets;
}

Defaults PTRepo::GetDefaults() const {
    QSqlQuery query(*db_);
    query.prepare("SELECT chambre, temperature, pressure, dref, duration_factor, stripper_response_correction, seance_duration FROM Defaults");

    if (!query.exec()) {
        qWarning() << "PTRepo::GetDefaults Query failed: " << query.lastError();
        throw std::runtime_error("Failed fetching default values from db");
    }

    if (!query.last()) {
        qWarning() << "PTRepo::GetDefaults No default values found in db";
        throw std::runtime_error("No default values found in db");
    }

    return Defaults(query.value(0).toString(), // chambre
                    query.value(1).toDouble(), // temperature
                    query.value(2).toDouble(), // pressure
                    query.value(3).toDouble(), // dref
                    query.value(4).toDouble(), // duration factor
                    query.value(5).toDouble(), // stripper response correction
                    query.value(6).toDouble()); // seance duration
}

QString PTRepo::GetOperatorPassword(const QString &username) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT password FROM Operator WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec()) {
        qWarning() << QString("PTRepo::GetOperatorPassword Query failed: ") << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    if (!query.next()) {
        qWarning() << "PTRepo::GetOperatorPassword No operator with username: " + username;
        throw std::runtime_error("Operator doesnt exist");
    }

    return query.value(0).toString();
}

QStringList PTRepo::GetOperators() const {
    QSqlQuery query(*db_);
    query.prepare("SELECT username FROM Operator");

    if (!query.exec()) {
        qWarning() << QString("PTRepo::GetOperators Query failed: ") << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    QStringList operators;
    while (query.next()) {
        operators.push_back(query.value(0).toString());
    }

    return operators;
}

void PTRepo::SaveCollimator(const Collimator &collimator) {
    if (!DossierExist(collimator.Dossier())) {
        QString message = QString("Le dossier %1 n'existe pas dans la base de données").arg(collimator.Dossier());
        throw std::runtime_error(message.toStdString());
    }
// Remove since it cause a lot of trouble for the dosimetrist. Added name comparison in gui instead
    auto dossier_patient = GetPatient(collimator.Dossier());
    if (!dossier_patient.SameNameAs(collimator.patient())) {
        QString message = QString("Le nom du patient pour le dossier %1 dans la base de données est différent: %2 %3")
                .arg(collimator.Dossier())
                .arg(dossier_patient.GetLastName())
                .arg(dossier_patient.GetFirstName());
        throw std::runtime_error(message.toStdString());
    }

    QSqlQuery query(*db_);
    query.prepare("INSERT INTO Collimateur(dossier, internal_edges) "
                  " VALUES(:dossier, :internal_edges)");
    query.bindValue(":dossier", collimator.Dossier());
    query.bindValue(":internal_edges", collimator.InternalEdgesPathFormat());

    if (!query.exec()) {
        qWarning() << "PTRepo::SaveCollimator Query failed: " << query.lastError();
        throw std::runtime_error("Echec de query exec");
    }
}

void PTRepo::SaveCollimateurItems(const Collimator &collimateur, const QString fraiseuse_program) {
    try {
        db_->transaction();

        SaveCollimator(collimateur);
        SaveTreatmentFraiseuseProgram(collimateur.Dossier(), Collimator::program_type, QDateTime::currentDateTime(), fraiseuse_program);

        if (!db_->commit()) {
            qWarning() << "PTRepo::SaveCollimateurItems Commit failed";
            throw std::runtime_error("Echec de la commit");
        }
    }
    catch (...) {
        qWarning() << "PTRepo::SaveCollimateurItems Rolling back...";
        db_->rollback();
        throw;
    }
}

Collimator PTRepo::GetCollimator(int dossier) const {
    auto patient = GetPatient(dossier);

    QSqlQuery query(*db_);
    query.prepare("SELECT dossier, internal_edges FROM Collimateur WHERE dossier = :dossier"
                  " ORDER BY collimateur_id ASC");
    query.bindValue(":dossier", dossier);

    if (!query.exec()) {
        qWarning() << "Query for collimator failed: " << query.lastError();
        throw std::runtime_error("Echec de la récupération du collimateur de la base de données");
    }

    if (!query.last()) {
        qWarning() << "PTRepo::GetCollimator No data found";
        QString message = QString("Aucun collimateur trouvé pour dossier %1 dans la base de données").arg(dossier);
        throw std::runtime_error(message.toStdString());
    }

    return Collimator(dossier, patient, query.value(1).toString());
}

void PTRepo::SaveCompensateurItems(const Compensateur &compensateur, const QString fraiseuse_program) {
    try {
        db_->transaction();

        SaveCompensateur(compensateur);
        SaveTreatmentFraiseuseProgram(compensateur.dossier(), Compensateur::program_type, QDateTime::currentDateTime(), fraiseuse_program);

        if (!db_->commit()) {
            qWarning() << "PTRepo::SaveCompensateurItems Commit failed";
            throw std::runtime_error("Db query failed");
        }
    }
    catch (...) {
        qWarning() << "PTRepo::SaveCompensateurItems Rolling back...";
        db_->rollback();
        throw;
    }
}

void PTRepo::SaveCompensateur(const Compensateur& compensateur) {
    if (!DossierExist(compensateur.dossier())) {
        throw std::runtime_error("Cannot save compensateur, the dossier does not exist");
    }

    QSqlQuery query(*db_);
    query.prepare("INSERT INTO Compensateur(dossier, diameter_eye, sclere_thickness, center_point, bolus_thickness, "
                  "parcours, parcours_error, error_radius_scaling, base_thickness) "
                  "VALUES(:dossier, :diameter_eye, :sclere_thickness, :center_point, :bolus_thickness, "
                  ":parcours, :parcours_error, :error_radius_scaling, :base_thickness)");
    query.bindValue(":dossier", compensateur.dossier());
    query.bindValue(":diameter_eye", compensateur.eye().diameter());
    query.bindValue(":sclere_thickness", compensateur.eye().sclere_thickness());
    query.bindValue(":center_point", dbutil::QCPCurveDataToPSQLPoint(compensateur.center_point()));
    query.bindValue(":bolus_thickness", compensateur.bolus_thickness());
    query.bindValue(":parcours", compensateur.parcours());
    query.bindValue(":parcours_error", compensateur.parcours_error());
    query.bindValue(":error_radius_scaling", compensateur.error_scaling());
    query.bindValue(":base_thickness", compensateur.base_thickness());

    if (!query.exec()) {
        qWarning() << "PTRepo::SaveCompensateur Query failed: " << query.lastError();
        throw std::runtime_error("Saving compensateur to db failed");
    }
}

Compensateur PTRepo::GetCompensateur(int dossier) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT dossier, diameter_eye, sclere_thickness, center_point, bolus_thickness, "
                  "parcours, parcours_error, error_radius_scaling, base_thickness "
                  "FROM Compensateur WHERE dossier = :dossier "
                  "ORDER BY compensateur_id ASC");
    query.bindValue(":dossier", dossier);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetCompensateur Query failed: " << query.lastError();
        throw std::runtime_error("Fetching compensateur from db failed");
    }

    if (!query.last()) {
        qWarning() << "PTRepo::GetCompensateur No data found";
        throw std::runtime_error("No collimateur found in db");
    }

    return Compensateur(dossier,
                        Eye(query.value(1).toDouble(), query.value(2).toDouble()),
                        dbutil::PSQLPointToQCPCurveData(query.value(3).toString()), // center point
                        query.value(4).toDouble(), // bolus thickness
                        query.value(5).toDouble(), // parcours
                        query.value(6).toDouble(), // parcours error
                        query.value(7).toDouble(), // error scaling
                        query.value(8).toDouble()); // base thickness

}

QStringList PTRepo::GetFraiseuseTreatmentTypes() const {
    QSqlQuery query(*db_);
    query.prepare("SELECT enumlabel FROM pg_enum WHERE enumtypid = "
                  "(SELECT oid FROM pg_type WHERE typname = :enum_name)");
    query.bindValue(":enum_name", QString("fraiseuse_treatment_types"));

    if (!query.exec()) {
        qWarning() << "PTRepo::GetFraiseuseTreatmentTypes Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    QStringList types;
    while (query.next()) {
        types.push_back(query.value(0).toString());
    }

    return types;
}

std::map<QDateTime, int> PTRepo::GetFraiseuseTreatmentPrograms(QString type) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT * FROM "
                  "(SELECT dossier, program_type, timestamp, ROW_NUMBER() OVER (PARTITION BY dossier, program_type ORDER BY timestamp DESC) col FROM TreatmentFraiseuseProgram) x "
                  "WHERE x.col = 1 AND program_type = :program_type");
    query.bindValue(":program_type", type);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetFraiseuseTreatmentPrograms Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    std::map<QDateTime, int> programs;
    while (query.next()) {
        programs.emplace(query.value(2).toDateTime(),
                         query.value(0).toInt());
    }

    return programs;
}

QString PTRepo::GetTreatmentFraiseuseProgram(int dossier, QString program_type) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT program FROM TreatmentFraiseuseProgram"
                  " WHERE dossier = :dossier AND program_type = :program_type"
                  " ORDER BY timestamp DESC LIMIT 1");
    query.bindValue(":dossier", dossier);
    query.bindValue(":program_type", program_type);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetTreatmentFraiseuseProgram Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    if (!query.last()) {
        qWarning() << "PTRepo::GetTreatmentFraiseuseProgram No data found";
        throw std::runtime_error("Db query returned no data");
    }

    return query.value(0).toString();
}

void PTRepo::SaveTreatmentFraiseuseProgram(int dossier, QString program_type, QDateTime timestamp, QString text) {
    QSqlQuery query(*db_);
    query.prepare("INSERT INTO TreatmentFraiseuseProgram(dossier, program_type, timestamp, program) "
                  " VALUES(:dossier, :program_type, :timestamp, :text)");
    query.bindValue(":dossier", dossier);
    query.bindValue(":program_type", program_type);
    query.bindValue(":timestamp", timestamp);
    query.bindValue(":text", text);

    if (!query.exec()) {
        qWarning() << "PTRepo::SaveTreatmentFraiseuseProgram Query failed: " << query.lastError();
        throw std::runtime_error("Echec de l'enregistrement du programme de fraisage");
    }
}


QStringList PTRepo::GetFraiseuseMiscTypes() const {
    QSqlQuery query(*db_);
    query.prepare("SELECT enumlabel FROM pg_enum WHERE enumtypid = "
                  "(SELECT oid FROM pg_type WHERE typname = :enum_name)");
    query.bindValue(":enum_name", QString("fraiseuse_misc_types"));

    if (!query.exec()) {
        qWarning() << "PTRepo::GetFraiseuseMiscTypes Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    QStringList types;
    while (query.next()) {
        types.push_back(query.value(0).toString());
    }

    return types;
}

std::map<QDateTime, QString> PTRepo::GetFraiseuseMiscPrograms(QString type) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT * FROM "
                  "(SELECT id, program_type, timestamp, ROW_NUMBER() OVER (PARTITION BY id, program_type ORDER BY timestamp DESC) col FROM MiscFraiseuseProgram) x "
                  "WHERE x.col = 1 AND program_type = :program_type");
    query.bindValue(":program_type", type);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetFraiseuseMiscPrograms Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    std::map<QDateTime, QString> programs;
    while (query.next()) {
        programs.emplace(query.value(2).toDateTime(),
                         query.value(0).toString());
    }

    return programs;
}


QString PTRepo::GetMiscFraiseuseProgram(QString id, QString program_type) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT program FROM MiscFraiseuseProgram"
                  " WHERE id = :id AND program_type = :program_type"
                  " ORDER BY timestamp DESC LIMIT 1");
    query.bindValue(":id", id);
    query.bindValue(":program_type", program_type);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetMiscFraiseuseProgram Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    if (!query.last()) {
        qWarning() << "PTRepo::GetMiscFraiseuseProgram No data found";
        throw std::runtime_error("Db query returned no data");
    }

    return query.value(0).toString();
}

void PTRepo::SaveMiscFraiseuseProgram(QString id, QString program_type, QDateTime timestamp, QString text) {
    QSqlQuery query(*db_);
    query.prepare("INSERT INTO MiscFraiseuseProgram(id, program_type, timestamp, program) "
                  " VALUES(:id, :program_type, :timestamp, :text)");
    query.bindValue(":id", id);
    query.bindValue(":program_type", program_type);
    query.bindValue(":timestamp", timestamp);
    query.bindValue(":text", text);

    if (!query.exec()) {
        qWarning() << "PTRepo::SaveMiscFraiseuseProgram Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }
}

MCNPXData PTRepo::GetMCNPXData() const {
    QSqlQuery query(*db_);
    query.prepare("SELECT um_plexi, monitor_units, depth_dose_array FROM MCNPXDoseCurve"
                  " ORDER BY um_plexi ASC");

    if (!query.exec()) {
        qWarning() << "PTRepo::GetMCNPXDoseCurves Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    std::vector<int> um_plexi;
    std::vector<double> monitor_units;
    std::vector<DepthDoseCurve> depth_dose_curves;
    while (query.next()) {
        um_plexi.push_back(query.value(0).toInt());
        monitor_units.push_back(query.value(1).toDouble());
        depth_dose_curves.push_back(dbutil::DecodeDepthDoseArray(query.value(2).toString()));
    }
    return MCNPXData(depth_dose_curves, um_plexi, monitor_units);
}

MCNPXData PTRepo::GetTheoreticalBraggData() const {
    QSqlQuery query(*db_);
    query.prepare("SELECT um_plexi, monitor_units, depth_dose_array FROM TheoreticalDoseCurve"
                  " ORDER BY um_plexi ASC");

    if (!query.exec()) {
        qWarning() << "PTRepo::GetTheoreticalDoseCurves Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    std::vector<int> um_plexi;
    std::vector<double> monitor_units;
    std::vector<DepthDoseCurve> depth_dose_curves;
    while (query.next()) {
        um_plexi.push_back(query.value(0).toInt());
        monitor_units.push_back(query.value(1).toDouble());
        depth_dose_curves.push_back(dbutil::DecodeDepthDoseArray(query.value(2).toString()));
    }
    return MCNPXData(depth_dose_curves, um_plexi, monitor_units);
}

TimedStampedDataSeries PTRepo::GetBeamTransmissionSeries(QDate from, QDate to) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT timestamp, avg(200 * i_cf9 / i_stripper), stddev(200 * i_cf9 / i_stripper) "
                  "FROM dosimetryrecord INNER JOIN dosimetry ON dosimetryrecord.dosimetry_id = dosimetry.dosimetry_id "
                  "WHERE dosimetry.dossier = 10000 AND Date(timestamp) >= :from AND Date(timestamp) <= :to AND i_stripper > 0 "
                  "GROUP BY dosimetry.dosimetry_id "
                  "ORDER BY timestamp ASC");
    query.bindValue(":from", from);
    query.bindValue(":to", to);

    if (!query.exec()) {
        qWarning() << "PTRepo::GetBeamTransmissionSeries Query failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    TimedStampedDataSeries data;
    while (query.next()) {
        data.time.push_back(query.value(0).toDateTime().toTime_t());
        data.value.push_back(query.value(1).toDouble());
        data.error.push_back(query.value(2).toDouble());
    }
    return data;
}

bool PTRepo::CheckConnection() {
    if (disconnect_on_idle_timeout_ > 0) disconnect_timer_.start();
    if (db_->isOpen()) {
        return true;
    } else {
        return this->Connect();
    }
}

bool PTRepo::CheckReadWriteAccess() {
    const QDateTime timestamp = QDateTime::currentDateTime();

    QSqlQuery w_query(*db_);
    w_query.prepare("INSERT INTO CheckConnection(timestamp)"
                  " VALUES(:timestamp)");
    w_query.bindValue(":timestamp", timestamp);

    if (!w_query.exec()) {
        qWarning() << "PTRepo::CheckReadWriteAccess Query failed: " << w_query.lastError();
        return false;
    }

    QSqlQuery r_query(*db_);
    r_query.prepare("SELECT timestamp FROM CheckConnection ORDER BY id DESC LIMIT 1");

    if (!r_query.exec()) {
        qWarning() << "PTRepo::CheckReadWriteAccess Query failed: " << r_query.lastError();
        return false;
    }

    if (r_query.size() != 1) {
        qWarning() << "PTRepo::CheckReadWriteAccess Did not return a single row: " << r_query.size();
        return false;
    }

    if (!r_query.next()) {
        qWarning() << "PTRepo::CheckReadWriteAccess No data found";
        return false;
    }

    return (r_query.value(0).toDateTime() == timestamp);
}





