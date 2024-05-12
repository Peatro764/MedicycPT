#include "QARepo.h"

#include <iostream>
#include <QtDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <stdexcept>
#include <QSqlDriver>
#include <QMetaMethod>

#include "Axis.h"
#include "Hardware.h"

QARepo::QARepo(const QString& conn_name,
                           const QString& hostname,
                           const int& port,
                           const QString& db_name,
                           const QString& username,
                           const QString& password)
    : QARepo(conn_name, hostname, port, db_name, username, password, 0) {}

QARepo::QARepo(const QString& conn_name,
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
    if (disconnect_on_idle_timeout_ > 0) {
        disconnect_timer_.setInterval(disconnect_on_idle_timeout * 1000);
        QObject::connect(&disconnect_timer_, &QTimer::timeout, this, &QARepo::Disconnect);
        disconnect_timer_.start();
    }
}

QARepo::~QARepo() {
    this->Disconnect();
    if (db_) delete db_;
    QSqlDatabase::removeDatabase(conn_name_);
}

bool QARepo::Connect() {
    if (db_->isOpen()) {
        return true;
    } else {
        emit (SIGNAL_OpeningConnection());
        db_->setConnectOptions("connect_timeout=5");//;keepalives=1;keepalives_idle=10;keepalives_interval=3;keepalives_count=3");
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

void QARepo::Disconnect() {
    if (db_->isOpen()) {
        db_->close();
        emit (SIGNAL_ConnectionClosed());
    }
}

SensorConfig QARepo::GetSensorConfig(Axis axis) const {
    return GetSensorConfig(AxisToString(axis));
}

SensorConfig QARepo::GetSensorConfig(SensorType type) const {
    QString dev_name = "UNK";
    switch (type) {
    case SensorType::DIODE:
        dev_name = "DIODE";
        break;
    case SensorType::CHAMBRE:
        dev_name = "CHAMBRE";
        break;
    case SensorType::POSITION_X:
        dev_name = "X";
        break;
    case SensorType::POSITION_Y:
        dev_name = "Y";
        break;
    case SensorType::POSITION_Z:
        dev_name = "Z";
        break;
    default:
        dev_name = "UNK";
        break;
    }

    return GetSensorConfig(dev_name);
}

SensorConfig QARepo::GetSensorConfig(QString dev_name) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT single_ended_channel, range, resolution_index, settling_us, channel, unit_per_volt, unit_offset "
                  "FROM SensorConfig WHERE name = :name");
    query.bindValue(":name", dev_name);

    if (!query.exec()) {
        qWarning() << "QARepo::GetSensorConfig Failed executing query: " << query.lastError();
        throw std::runtime_error("Failed retrieving sensor config from db");
    }

    if (!query.next()) {
        qWarning() << "QARepo::GetSensorConfig Call next failed";
        throw std::runtime_error("Failed retrieving sensor config from db");
    }

    return SensorConfig(query.value(0).toInt(),
                        query.value(1).toDouble(),
                        query.value(2).toInt(),
                        query.value(3).toInt(),
                        static_cast<t7::AIN>(query.value(4).toInt()),
                        query.value(5).toDouble(),
                        query.value(6).toDouble());
}

MotorConfig QARepo::GetMotorConfig(Axis axis, QA_HARDWARE hardware) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT ch_full_or_half_step, ch_direction, ch_preset, ch_pulse, low_to_high_transition_count, high_to_low_transition_count, "
                  "n_pulses_per_step, distance_per_step, distance_offset, msec_per_step_constants_default, msec_per_step_constants_fast, invert_direction "
                   "FROM MotorConfig WHERE name = :name AND type = :type");
    query.bindValue(":name", AxisToString(axis));
    query.bindValue(":type", HardwareToString(hardware));

    if (!query.exec()) {
        qWarning() << "QARepo::GetMotorConfig Failed executing query: " << query.lastError();
        throw std::runtime_error("Failed retrieving motor config from db");
    }

    if (!query.next()) {
        qWarning() << "QARepo::GetMotorConfig Call next failed";
        throw std::runtime_error("Failed retrieving motor config from db");
    }

    return MotorConfig(static_cast<t7::DIO>(query.value(0).toInt()),
                       static_cast<t7::DIO>(query.value(1).toInt()),
                       static_cast<t7::DIO>(query.value(2).toInt()),
                       static_cast<t7::PULSEOUT>(query.value(3).toInt()),
                       query.value(4).toInt(),
                       query.value(5).toInt(),
                       query.value(6).toInt(),
                       query.value(7).toDouble(),
                       query.value(8).toDouble(),
                       DecodeDoubleArray(query.value(9).toString()),
                       DecodeDoubleArray(query.value(10).toString()),
                       query.value(11).toBool());
}

ClockConfig QARepo::GetClockConfig(QA_HARDWARE hardware) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT channel, divisor, roll_value FROM ClockConfig WHERE type = :type");
    query.bindValue(":type", HardwareToString(hardware));

    if (!query.exec()) {
        qWarning() << "QARepo::GetClockConfig Failed executing query: " << query.lastError();
        throw std::runtime_error("Failed retrieving clock config from db");
    }

    if (!query.next()) {
        qWarning() << "QARepo::GetClockConfig Call next failed";
        throw std::runtime_error("Failed retrieving clock config from db");
    }

    return ClockConfig(static_cast<t7::CLOCK>(query.value(0).toInt()),
                       query.value(1).toInt(),
                       query.value(2).toInt());
}

void QARepo::SaveBeamProfile(const BeamProfile& profile) {
    const QString measurement_type("PROFILE");
    try {
        const BeamProfileResults results = profile.GetResults();

        db_->transaction();
        QSqlQuery query(*db_);
        query.prepare("INSERT INTO BeamProfile(timestamp, hardware, axis, smoothing_elements, centre, width_95, width_90, penumbra, flatness, noise) "
                      "VALUES(:timestamp, :hardware, :axis, :smoothing_elements, :centre, :width_95, :width_90, :penumbra, :flatness, :noise) "
                      "RETURNING beam_profile_id");
        query.bindValue(":timestamp", profile.GetTimestamp());
        query.bindValue(":hardware", HardwareToString(profile.Hardware()));
        query.bindValue(":axis", AxisToString(profile.GetAxis()));
        query.bindValue(":smoothing_elements", profile.GetSmoothingElements());
        query.bindValue(":centre", results.centre());
        query.bindValue(":width_95", results.width_95());
        query.bindValue(":width_90", results.width_90());
        query.bindValue(":penumbra", results.penumbra());
        query.bindValue(":flatness", results.flatness());
        query.bindValue(":noise", profile.GetSignalNoise());

        if (!query.exec()) {
            qWarning() << "QARepo::SaveBeamProfile Failed saving to db: " << query.lastError();
            throw std::runtime_error("Failed saving beamprofile to db");
        }

        if (!query.next()) {
            qWarning() << "QARepo::SaveBeamProfile Could not get beam_profile_id after inserting";
            throw std::runtime_error("Failed saving beam profile to db");
        }

        const int measurement_id = query.value(0).toInt();
        this->SaveMeasurementPoints(measurement_type, measurement_id, profile.GetPoints());
        this->SaveMeasurementCurrents(measurement_type, measurement_id, profile.GetCurrents());

        if (!db_->commit()) {
            qWarning() << "QARepo::SaveBeamProfile Commit failed";
            throw std::runtime_error("Failed saving beam profile to db");
        }
    }
    catch (...) {
        qWarning() << "QARepo::SaveBeamProfile Rolling back commit";
        db_->rollback();
        throw;
    }
}

std::vector<BeamProfile> QARepo::GetBeamProfiles(Axis axis, const QDate& from, const QDate& to) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT timestamp FROM BeamProfile "
                  "WHERE axis = :axis AND Date(timestamp) >= :from AND Date(timestamp) <= :to");
    query.bindValue(":axis", AxisToString(axis));
    query.bindValue(":from", from);
    query.bindValue(":to", to);

    if (!query.exec()) {
        qWarning() << "QARepo::GetBeamProfiles Failed getting beamprofiles: " << query.lastError();
        throw std::runtime_error("Failed retrieving beamprofiles from db");
    }

    std::vector<BeamProfile> profiles;
    while (query.next()) {
        profiles.push_back(GetBeamProfile(axis, query.value(0).toDateTime()));
    }
    return profiles;
}

void QARepo::UpdateBeamProfile(const BeamProfile& profile) {
    QSqlQuery query(*db_);
    query.prepare("UPDATE BeamProfile SET flatness = :flatness "
                  "WHERE timestamp = :timestamp AND axis = :axis");
    query.bindValue(":flatness", profile.Flatness());
    query.bindValue(":timestamp", profile.GetTimestamp());
    query.bindValue(":axis", AxisToString(profile.GetAxis()));

    if (!query.exec()) {
        qWarning() << "QARepo::UpdateBeamProfile Failed saving to db: " << query.lastError();
        throw std::runtime_error("Failed updating beamprofile from db");
    }
}

BeamProfile QARepo::GetBeamProfile(Axis axis, const QDateTime &timestamp) const {
    const QString measurement_type("PROFILE");
    QSqlQuery query(*db_);
    query.prepare("SELECT beam_profile_id, timestamp, hardware, axis, smoothing_elements, noise FROM BeamProfile "
                  "WHERE timestamp = :timestamp AND axis = :axis");
    query.bindValue(":timestamp", timestamp);
    query.bindValue(":axis", AxisToString(axis));

    if (!query.exec()) {
        qWarning() << "QARepo::GetBeamProfile Failed saving to db: " << query.lastError();
        throw std::runtime_error("Failed retrieving beamprofile from db");
    }

    if (!query.next()) {
        qWarning() << "QARepo::GetBeamProfile Call next failed";
        throw std::runtime_error("Failed retrieving beamprofile from db");
    }

    const int measurement_id = query.value(0).toInt();

    MeasurementCurrents currents = this->GetMeasurementCurrents(measurement_type, measurement_id);
    auto points = this->GetMeasurementPoints(measurement_type, measurement_id);

    return BeamProfile(StringToHardware(query.value(2).toString()), axis, points,
                       query.value(1).toDateTime(), // timestamp
                       currents,
                       query.value(4).toInt(), // smoothing_elements
                       query.value(5).toDouble()); // noise
}

BeamProfile QARepo::GetLastBeamProfile(Axis axis) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT timestamp FROM BeamProfile WHERE axis = :axis AND Date(timestamp) = :date "
                  "ORDER BY TIMESTAMP DESC LIMIT 1");
    query.bindValue(":axis", AxisToString(axis));
    query.bindValue(":date", QDateTime::currentDateTime().date());

    if (!query.exec()) {
        qWarning() << "QARepo::GetLastBeamProfile Failed saving to db: " << query.lastError();
        throw std::runtime_error("Failed retrieving last beamprofile from db");
    }

    if (!query.next()) {
        qWarning() << "QARepo::GetLastBeamProfile Call next failed";
        throw std::runtime_error("Failed retrieving last beamprofile from db");
    }

    const auto timestamp = query.value(0).toDateTime();
    return GetBeamProfile(axis, timestamp);
}

BeamProfileSeries QARepo::GetBeamProfileSeries(Axis axis, QDate from, QDate to) const {
    const QString measurement_type("PROFILE");
    QSqlQuery query(*db_);
    query.prepare("SELECT beam_profile_id, timestamp, axis, centre, width_95, width_90, penumbra, flatness"
                  " FROM BeamProfile WHERE Date(timestamp) >= :from AND Date(timestamp) <= :to AND axis = :axis"
                  " ORDER BY timestamp ASC");
    query.bindValue(":from", from);
    query.bindValue(":to", to);
    query.bindValue(":axis", AxisToString(axis));

    if (!query.exec()) {
        qWarning() << "QARepo::GetBeamProfileSeries Query failed: " << query.lastError();
        throw std::runtime_error("Failed retreiving beam profile series");
    }

    BeamProfileSeries series(axis);
    while(query.next()) {
        const int measurement_id = query.value(0).toInt();
        MeasurementCurrents currents = this->GetMeasurementCurrents(measurement_type, measurement_id);
        series.AddPoint(query.value(1).toDateTime(),
                        currents,
                        query.value(4).toDouble(),
                        query.value(5).toDouble(),
                        query.value(6).toDouble(),
                        query.value(7).toDouble());

    }
    return series;
}

void QARepo::SaveBraggPeak(const BraggPeak& braggpeak) {
    const QString measurement_type("BRAGGPEAK");
    const DepthDoseMeasurement depth_dose = braggpeak.depth_dose();

    try {
        db_->transaction();
        QSqlQuery query(*db_);
        query.prepare("INSERT INTO BraggPeak(timestamp, hardware, smoothing_elements, width_50, penumbra, parcours, noise) "
                      "VALUES(:timestamp, :hardware, :smoothing_elements, :width_50, :penumbra, :parcours, :noise) "
                      "RETURNING bragg_peak_id");
        query.bindValue(":timestamp", depth_dose.GetTimestamp());
        query.bindValue(":hardware", HardwareToString(depth_dose.Hardware()));
        query.bindValue(":smoothing_elements", depth_dose.GetSmoothingElements());
        query.bindValue(":width_50", braggpeak.width50());
        query.bindValue(":penumbra", braggpeak.penumbra());
        query.bindValue(":parcours", braggpeak.parcours());
        query.bindValue(":noise", depth_dose.GetSignalNoise());

        if (!query.exec()) {
            qWarning() << "QARepo::SaveBraggPeak Failed saving to db: " << query.lastError();
            throw std::runtime_error("Failed saving braggpeak to db");
        }

        if (!query.next()) {
            qWarning() << "QARepo::SaveBraggPeak Could not get bragg_peak_id after inserting";
            throw std::runtime_error("Failed saving braggpeak to db");
        }

        const int measurement_id = query.value(0).toInt();
        this->SaveMeasurementPoints(measurement_type, measurement_id, depth_dose.GetPoints());
        this->SaveMeasurementCurrents(measurement_type, measurement_id, depth_dose.GetCurrents());

        if (!db_->commit()) {
            qWarning() << "QARepo::SaveBraggPeak Commit failed";
            throw std::runtime_error("Failed saving bragg peak to db");
        }
    }
    catch (...) {
        qWarning() << "QARepo::SaveBraggPeak Rolling back commit";
        db_->rollback();
        throw;
    }
}

std::vector<BraggPeak> QARepo::GetBraggPeaks(QDate from, QDate to) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT timestamp FROM BraggPeak "
                  "WHERE Date(timestamp) >= :from AND Date(timestamp) <= :to");
    query.bindValue(":from", from);
    query.bindValue(":to", to);

    if (!query.exec()) {
        qWarning() << "QARepo::GetBraggPeaks Failed saving to db: " << query.lastError();
        throw std::runtime_error("Failed retrieving braggpeak from db");
    }

    std::vector<BraggPeak> braggpeaks;
    while (query.next()) {
        braggpeaks.push_back(GetBraggPeak(query.value(0).toDateTime()));
    }
    return braggpeaks;
}


BraggPeak QARepo::GetBraggPeak(const QDateTime& timestamp) const {
    const QString measurement_type("BRAGGPEAK");
    QSqlQuery query(*db_);
    query.prepare("SELECT bragg_peak_id, timestamp, hardware, smoothing_elements, noise, "
                  "width_50, penumbra, parcours FROM BraggPeak "
                  "WHERE timestamp = :timestamp");
    query.bindValue(":timestamp", timestamp);

    if (!query.exec()) {
        qWarning() << "QARepo::GetBraggPeak Failed saving to db: " << query.lastError();
        throw std::runtime_error("Failed retrieving braggpeak from db");
    }

    if (!query.next()) {
        qWarning() << "QARepo::GetBraggPeak Call next failed";
        throw std::runtime_error("Failed retrieving braggpeak from db");
    }

    const int measurement_id = query.value(0).toInt();

    MeasurementCurrents currents = this->GetMeasurementCurrents(measurement_type, measurement_id);
    auto points = this->GetMeasurementPoints(measurement_type, measurement_id);
    DepthDoseMeasurement depth_dose(StringToHardware(query.value(2).toString()),
                         points,
                         query.value(1).toDateTime(),
                         currents,
                         query.value(3).toInt(),
                         query.value(4).toDouble());

    return BraggPeak(depth_dose,
                     query.value(5).toDouble(), // width50
                     query.value(6).toDouble(), // penumbra
                     query.value(7).toDouble()); // parcours
}

BraggPeak QARepo::GetLastBraggPeak() const {
    QSqlQuery query(*db_);
    query.prepare("SELECT timestamp FROM BraggPeak WHERE Date(timestamp) = :date "
                  "ORDER BY TIMESTAMP DESC LIMIT 1");
    query.bindValue(":date", QDateTime::currentDateTime().date());

    if (!query.exec()) {
        qWarning() << "QARepo::GetLastBraggPeak Failed saving to db: " << query.lastError();
        throw std::runtime_error("Failed retrieving last braggpeak from db");
    }

    if (!query.next()) {
        qWarning() << "QARepo::GetLastBraggPeak Call next failed";
        throw std::runtime_error("Failed retrieving last braggPeak from db");
    }

    const auto timestamp = query.value(0).toDateTime();
    return GetBraggPeak(timestamp);
}


void QARepo::SaveSOBP(const SOBP& sobp) {
    const QString measurement_type("SOBP");
    const DepthDoseMeasurement depth_dose = sobp.depth_dose();

    try {
        db_->transaction();
        QSqlQuery query(*db_);
        query.prepare("INSERT INTO SOBP(timestamp, hardware, smoothing_elements, dossier, modulateur_id, degradeur_mm, comment, penumbra, parcours, modulation_98, modulation_100, noise) "
                      "VALUES(:timestamp, :hardware, :smoothing_elements, :dossier, :modulateur_id, :degradeur_mm, :comment, :penumbra, :parcours, :modulation_98, :modulation_100, :noise) "
                      "RETURNING sobp_id");
        query.bindValue(":timestamp", depth_dose.GetTimestamp());
        query.bindValue(":hardware", HardwareToString(depth_dose.Hardware()));
        query.bindValue(":smoothing_elements", depth_dose.GetSmoothingElements());
        if (sobp.dossier() == 0) {
            query.bindValue(":dossier", QVariant(QVariant::Int));
        } else {
            query.bindValue(":dossier", sobp.dossier());
        }
        query.bindValue(":modulateur_id", sobp.modulateur_id());
        query.bindValue(":degradeur_mm", sobp.mm_degradeur());
        query.bindValue(":comment", sobp.comment());
        query.bindValue(":penumbra", sobp.penumbra());
        query.bindValue(":parcours", sobp.parcours());
        query.bindValue(":modulation_98", sobp.mod98());
        query.bindValue(":modulation_100", sobp.mod100());
        query.bindValue(":noise", depth_dose.GetSignalNoise());

        if (!query.exec()) {
            qWarning() << "QARepo::SaveSOBP Failed saving to db: " << query.lastError();
            throw std::runtime_error("Failed saving sobp to db");
        }

        if (!query.next()) {
            qWarning() << "QARepo::SaveSOBP Could not get sobp_id after inserting";
            throw std::runtime_error("Failed saving sobp to db");
        }

        const int measurement_id = query.value(0).toInt();
        this->SaveMeasurementPoints(measurement_type, measurement_id, depth_dose.GetPoints());
        this->SaveMeasurementCurrents(measurement_type, measurement_id, depth_dose.GetCurrents());

        if (!db_->commit()) {
            qWarning() << "QARepo::SaveSOBP Commit failed";
            throw std::runtime_error("Failed saving SOBP to db");
        }
    }
    catch (...) {
        qWarning() << "QARepo::SaveSOBP Rolling back commit";
        db_->rollback();
        throw;
    }
}

SOBP QARepo::GetLastSOBP() const {
    QSqlQuery query(*db_);
    query.prepare("SELECT timestamp FROM SOBP WHERE Date(timestamp) = :date "
                  "ORDER BY TIMESTAMP DESC LIMIT 1");
    query.bindValue(":date", QDateTime::currentDateTime().date());

    if (!query.exec()) {
        qWarning() << "QARepo::GetLastSOBP Failed saving to db: " << query.lastError();
        throw std::runtime_error("Failed retrieving last sobp from db");
    }

    if (!query.next()) {
        qWarning() << "QARepo::GetLastSOBP Call next failed";
        throw std::runtime_error("Failed retrieving last sobp from db");
    }

    const auto timestamp = query.value(0).toDateTime();
    return GetSOBP(timestamp);
}

SOBP QARepo::GetSOBPForDossier(int dossier) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT timestamp FROM SOBP WHERE dossier = :dossier "
                  "ORDER BY TIMESTAMP DESC LIMIT 1");
    query.bindValue(":dossier", dossier);

    if (!query.exec()) {
        qWarning() << "QARepo::GetSOBPForDossier Failed saving to db: " << query.lastError();
        throw std::runtime_error("Failed retrieving sobp for dossier from db");
    }

    if (!query.next()) {
        qWarning() << "QARepo::GetSOBPForDossier Call next failed";
        throw std::runtime_error("Failed retrieving sobp for dossier from db");
    }

    const auto timestamp = query.value(0).toDateTime();
    return GetSOBP(timestamp);
}


SOBP QARepo::GetSOBP(const QDateTime& timestamp) const {
    const QString measurement_type("SOBP");
    QSqlQuery query(*db_);
    query.prepare("SELECT sobp_id, timestamp, hardware, smoothing_elements, dossier, modulateur_id, degradeur_mm, comment, "
                  "noise, penumbra, parcours, modulation_98, modulation_100 FROM SOBP "
                  "WHERE timestamp = :timestamp");
    query.bindValue(":timestamp", timestamp);

    if (!query.exec()) {
        qWarning() << "QARepo::GetSOBP Failed saving to db: " << query.lastError();
        throw std::runtime_error("Failed retrieving sobp from db");
    }

    if (!query.next()) {
        qWarning() << "QARepo::GetSOBP Call next failed";
        throw std::runtime_error("Failed retrieving sobp from db");
    }

    const int measurement_id = query.value(0).toInt();

    MeasurementCurrents currents = this->GetMeasurementCurrents(measurement_type, measurement_id);
    auto points = this->GetMeasurementPoints(measurement_type, measurement_id);

    DepthDoseMeasurement depth_dose(StringToHardware(query.value(2).toString()), // hardware
                         points, query.value(1).toDateTime(), currents,
                         query.value(3).toInt(), // smoothing_elements
                         query.value(8).toDouble()); // noise

    return SOBP(depth_dose,
                query.isNull(4) ? 0 : query.value(4).toInt(), // dossier
                query.value(5).toInt(), // modulateur_id
                query.value(6).toDouble(), // degradeur_mm
                query.value(7).toString(), // comment
                query.value(9).toDouble(), // penumbra
                query.value(10).toDouble(), // parcours
                query.value(11).toDouble(), // mod98
                query.value(12).toDouble()); // mod100
}

std::vector<SOBP> QARepo::GetSOBPs() const {
    const QString measurement_type("SOBP");
    QSqlQuery query(*db_);
    query.prepare("SELECT timestamp FROM SOBP "
                  "WHERE dossier != 10000");

    if (!query.exec()) {
        qWarning() << "QARepo::GetSOBPs Failed saving to db: " << query.lastError();
        throw std::runtime_error("Failed retrieving sobps from db");
    }

    std::vector<SOBP> sobps;
    while (query.next()) {
        sobps.push_back(this->GetSOBP(query.value(0).toDateTime()));
    }
    return sobps;
}


std::map<QDateTime, ModAcc> QARepo::GetSOBPSForModulateur(int modulateur_id) const {
    return GetSOBPSForModulateur(modulateur_id, QDate(1000, 1, 1), QDate(3000, 1, 1));
}

std::map<QDateTime, ModAcc> QARepo::GetSOBPSForModulateur(int modulateur_id, QDate from, QDate to) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT timestamp, dossier, modulateur_id, degradeur_mm, comment FROM SOBP "
                  "WHERE modulateur_id = :modulateur_id AND "
                  "Date(timestamp) >= :from AND Date(timestamp) <= :to");
    query.bindValue(":modulateur_id", modulateur_id);
    query.bindValue(":from", from);
    query.bindValue(":to", to);

    if (!query.exec()) {
        qWarning() << "QARepo::GetSOBP Failed saving to db: " << query.lastError();
        throw std::runtime_error("Failed retrieving braggpeak from db");
    }

    std::map<QDateTime, ModAcc> sobps;
    while (query.next()) {
        sobps.emplace(query.value(0).toDateTime(),
                      ModAcc(query.value(1).toInt(),
                             query.value(2).toInt(),
                             query.value(3).toDouble(),
                             query.value(4).toString()));
    }
    return sobps;
}

SOBPSeries QARepo::GetSOBPSeries(int modulator, QDate from, QDate to) const {
    const QString measurement_type("SOBP");
    QSqlQuery query(*db_);
    query.prepare("SELECT sobp_id, timestamp, penumbra, parcours, modulation_98, modulation_100"
                  " FROM SOBP WHERE Date(timestamp) >= :from AND Date(timestamp) <= :to"
                  " AND modulateur_id = :modulateur_id"
                  " ORDER BY timestamp ASC");
    query.bindValue(":from", from);
    query.bindValue(":to", to);
    query.bindValue(":modulateur_id", modulator);

    if (!query.exec()) {
        qWarning() << "QARepo::GetBraggPeak Query failed: " << query.lastError();
        throw std::runtime_error("Failed retreiving braggpeak");
    }

    SOBPSeries series;
    while(query.next()) {
        const int measurement_id = query.value(0).toInt();
        MeasurementCurrents currents = this->GetMeasurementCurrents(measurement_type, measurement_id);
        series.AddPoint(query.value(1).toDateTime(),
                        currents,
                        query.value(2).toDouble(),
                        query.value(3).toDouble(),
                        query.value(4).toDouble(), // mod98
                        query.value(5).toDouble()); // mod100
    }
    return series;
}


std::vector<int> QARepo::GetSOBPModulateurs() const {
    QSqlQuery query(*db_);
    query.prepare("SELECT DISTINCT modulateur_id FROM SOBP WHERE modulateur_id != 10000 ORDER BY modulateur_id ASC");

    if (!query.exec()) {
        qWarning() << "QARepo::GetSOBPModulateurs Failed saving to db: " << query.lastError();
        throw std::runtime_error("Failed retrieving sobp modulateurs from db");
    }

    std::vector<int> modulateurs;
    while (query.next()) {
        modulateurs.push_back(query.value(0).toInt());
    }
    return modulateurs;
}

void QARepo::SaveCube(const CuveCube& cube) {
    const QString measurement_type("CUBE");

    try {
        db_->transaction();
        QSqlQuery query(*db_);
        query.prepare("INSERT INTO CUBE(timestamp, comment, x_nbins, x_min, x_step, "
                      "y_nbins, y_min, y_step, z_nbins, z_min, z_step, noise) "
                      "VALUES(:timestamp, :comment, :x_nbins, :x_min, :x_step, "
                      ":y_nbins, :y_min, :y_step, :z_nbins, :z_min, :z_step, :noise) "
                      "RETURNING cube_id");
        query.bindValue(":timestamp", cube.timestamp());
        query.bindValue(":comment", cube.comment());
        query.bindValue(":x_nbins", cube.GetAxisConfig(Axis::X).nbins());
        query.bindValue(":x_min", cube.GetAxisConfig(Axis::X).min());
        query.bindValue(":x_step", cube.GetAxisConfig(Axis::X).step());
        query.bindValue(":y_nbins", cube.GetAxisConfig(Axis::Y).nbins());
        query.bindValue(":y_min", cube.GetAxisConfig(Axis::Y).min());
        query.bindValue(":y_step", cube.GetAxisConfig(Axis::Y).step());
        query.bindValue(":z_nbins", cube.GetAxisConfig(Axis::Z).nbins());
        query.bindValue(":z_min", cube.GetAxisConfig(Axis::Z).min());
        query.bindValue(":z_step", cube.GetAxisConfig(Axis::Z).step());
        query.bindValue(":noise", cube.GetNoise());

        if (!query.exec()) {
            qWarning() << "QARepo::SaveCube Failed saving to db: " << query.lastError();
            throw std::runtime_error("Failed saving cube to db");
        }

        if (!query.next()) {
            qWarning() << "QARepo::SaveCube Could not get cube_id after inserting";
            throw std::runtime_error("Failed saving cube to db");
        }

        const int measurement_id = query.value(0).toInt();
        this->SaveMeasurementPoints(measurement_type, measurement_id, cube.GetMeasurementPoints());

        if (!db_->commit()) {
            qWarning() << "QARepo::SaveCube Commit failed";
            throw std::runtime_error("Failed saving Cube to db");
        }
    }
    catch (...) {
        qWarning() << "QARepo::SaveCube Rolling back commit";
        db_->rollback();
        throw;
    }
}

CuveCube QARepo::GetCube(const QDateTime& timestamp) const {
    const QString measurement_type("CUBE");
    QSqlQuery query(*db_);
    query.prepare("SELECT cube_id, timestamp, comment, x_nbins, x_min, x_step, "
                  "y_nbins, y_min, y_step, z_nbins, z_min, z_step, noise FROM CUBE "
                  "WHERE timestamp = :timestamp");
    query.bindValue(":timestamp", timestamp);

    if (!query.exec()) {
        qWarning() << "QARepo::GetCUBE Failed saving to db: " << query.lastError();
        throw std::runtime_error("Failed retrieving cube from db");
    }

    if (!query.next()) {
        qWarning() << "QARepo::GetCUBE Call next failed";
        throw std::runtime_error("Failed retrieving cube from db");
    }

    const int measurement_id = query.value(0).toInt();

    auto points = this->GetMeasurementPoints(measurement_type, measurement_id);

    std::map<Axis, AxisConfig> configs;
    configs[Axis::X] = AxisConfig(query.value(3).toInt(), // x_nbins
                                  query.value(4).toDouble(), // x_min
                                  query.value(5).toDouble()); // x_step
    configs[Axis::Y] = AxisConfig(query.value(6).toInt(), // y_nbins
                                  query.value(7).toDouble(), // y_min
                                  query.value(8).toDouble()); // y_step
    configs[Axis::Z] = AxisConfig(query.value(9).toInt(), // z_nbins
                                  query.value(10).toDouble(), // z_min
                                  query.value(11).toDouble()); // z_step

    return CuveCube(query.value(1).toDateTime(), // timestamp
                    query.value(2).toString(), // comment
                    configs, points,
                    query.value(12).toDouble()); // noise
}

std::map<QDateTime, QString> QARepo::GetCubeKeys() const {
    QSqlQuery query(*db_);
    query.prepare("SELECT timestamp, comment FROM CUBE");

    if (!query.exec()) {
        qWarning() << "QARepo::GetCubeKeys Failed saving to db: " << query.lastError();
        throw std::runtime_error("Failed retrieving cube keys from db");
    }

    std::map<QDateTime, QString> keys;
    while (query.next()) {
        keys[query.value(0).toDateTime()] = query.value(1).toString();
    }

    return keys;
}


BraggPeakSeries QARepo::GetBraggPeakSeries(QDate from, QDate to) const {
    const QString measurement_type("BRAGGPEAK");
    QSqlQuery query(*db_);
    query.prepare("SELECT bragg_peak_id, timestamp, width_50, penumbra, parcours"
                  " FROM BraggPeak WHERE Date(timestamp) >= :from AND Date(timestamp) <= :to"
                  " ORDER BY timestamp ASC");
    query.bindValue(":from", from);
    query.bindValue(":to", to);

    if (!query.exec()) {
        qWarning() << "QARepo::GetBraggPeak Query failed: " << query.lastError();
        throw std::runtime_error("Failed retreiving braggpeak");
    }

    BraggPeakSeries series;
    while(query.next()) {
        const int measurement_id = query.value(0).toInt();
        MeasurementCurrents currents = this->GetMeasurementCurrents(measurement_type, measurement_id);
        series.AddPoint(query.value(1).toDateTime(),
                        currents,
                        query.value(2).toDouble(),
                        query.value(3).toDouble(),
                        query.value(4).toDouble());

    }
    return series;
}

//std::vector<BeamProfile> QARepo::GetBeamProfiles(Axis axis, const QDate& date) const {
//    QSqlQuery query(*db_);
//    query.prepare("SELECT beam_profile_id, timestamp, axis, smoothing_elements, i_stripper, i_cf9,"
//                  " i_chambre1, i_chambre2"
//                  " FROM BeamProfile WHERE Date(timestamp) = :date AND axis = :axis"
//                  " ORDER BY timestamp ASC");
//    query.bindValue(":axis", util::AxisToString(axis));
//    query.bindValue(":date", date);

//    if (!query.exec()) {
//        qWarning() << "QARepo::GetBeamProfiles Failed executing query: " << query.lastError();
//        throw std::runtime_error("Failed retrieving beam profiles from db");
//    }

//    std::vector<BeamProfile> profiles;
//    while (query.next()) {
//        profiles.push_back(BeamProfile(util::StringToAxis(query.value(2).toString()),
//                                       GetBeamProfilePoints(query.value(0).toInt()), // beam_profile_id
//                                       query.value(3).toInt(), // smoothing_elements
//                                       query.value(4).toDouble(), // current stripper
//                                       query.value(5).toDouble(), // current cf9
//                                       query.value(6).toDouble(), // current chambre1
//                                       query.value(7).toDouble(), // current chambre2
//                                       query.value(1).toDateTime())); // datetime
//    }
//    return profiles;
//}

MeasurementCurrents QARepo::GetMeasurementCurrents(QString measurement_type, int measurement_id) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT timestamp, i_stripper, i_cf9, i_chambre1, i_chambre2, i_diode"
                  " FROM MeasurementCurrents WHERE measurement_type = :measurement_type AND measurement_id = :measurement_id");
    query.bindValue(":measurement_type", measurement_type);
    query.bindValue(":measurement_id", measurement_id);

    if (!query.exec()) {
        qWarning() << "QARepo::GetMeasurementCurrents Query failed: " << query.lastError();
        throw std::runtime_error("Failed retreiving measurement currents");
    }

    if (!query.next()) {
        qWarning() << "QARepo::GetMeasurementCurrents Call next failed";
        throw std::runtime_error("Failed retrieving measurement currents from db");
    }

    return MeasurementCurrents(query.value(0).toDateTime(),
                               query.value(1).toDouble(),
                               query.value(2).toDouble(),
                               query.value(3).toDouble(),
                               query.value(4).toDouble(),
                               query.value(5).toDouble());
}

void QARepo::SaveMeasurementCurrents(QString measurement_type, int measurement_id, const MeasurementCurrents& currents) {
    QSqlQuery query(*db_);
    query.prepare("INSERT INTO MeasurementCurrents(timestamp, measurement_type, measurement_id, i_stripper, i_cf9, i_chambre1, "
                  "i_chambre2, i_diode) "
                  "VALUES(:timestamp, :measurement_type, :measurement_id, :i_stripper, :i_cf9, :i_chambre1, "
                  ":i_chambre2, :i_diode)");
    query.bindValue(":timestamp", currents.timestamp());
    query.bindValue(":measurement_type", measurement_type);
    query.bindValue(":measurement_id", measurement_id);
    query.bindValue(":i_stripper", currents.stripper());
    query.bindValue(":i_cf9", currents.cf9());
    query.bindValue(":i_chambre1", currents.chambre1());
    query.bindValue(":i_chambre2", currents.chambre2());
    query.bindValue(":i_diode", currents.diode());

    if (!query.exec()) {
        qWarning() << "QARepo::SaveMeasurementCurrents Failed saving to db: " << query.lastError();
        throw std::runtime_error("Failed saving measurement currents to db");
    }
}

std::vector<MeasurementPoint> QARepo::GetMeasurementPoints(QString measurement_type, int measurement_id) const {
    QSqlQuery query(*db_);
    query.prepare("SELECT x, y, z, i_diode, i_chambre "
                  "FROM MeasurementPoint WHERE measurement_type = :measurement_type AND measurement_id = :measurement_id "
                  "ORDER BY id ASC");
    query.bindValue(":measurement_type", measurement_type);
    query.bindValue(":measurement_id", measurement_id);

    if (!query.exec()) {
        qWarning() << "QARepo::GetMeasurementPoints Query failed: " << query.lastError();
        throw std::runtime_error("Failed retreiving measurement points");
    }

    std::vector<MeasurementPoint> points;
    while (query.next()) {
        points.push_back(MeasurementPoint(Point(query.value(0).toDouble(),
                                                query.value(1).toDouble(),
                                                query.value(2).toDouble()), // z
                                          BeamSignal(query.value(3).toDouble(), // diode
                                                 query.value(4).toDouble()))); // chambre
    }
    return points;
}

void QARepo::SaveMeasurementPoints(QString measurement_type, int measurement_id, const std::vector<MeasurementPoint>& points) {
    QSqlQuery query(*db_);
    for (const MeasurementPoint &p : points) {
        query.clear();
        query.prepare("INSERT INTO MeasurementPoint(measurement_type, measurement_id, x, y, z, i_diode, i_chambre) "
                      "VALUES(:measurement_type, :measurement_id, :x, :y, :z, :i_diode, :i_chambre)");
        query.bindValue(":measurement_type", measurement_type);
        query.bindValue(":measurement_id", measurement_id);
        query.bindValue(":x", p.pos(Axis::X));
        query.bindValue(":y", p.pos(Axis::Y));
        query.bindValue(":z", p.pos(Axis::Z));
        query.bindValue(":i_diode", p.signal().raw());
        query.bindValue(":i_chambre", p.signal().reference());

        if (!query.exec()) {
            qWarning() << "QARepo::SaveMeasurementPoints Query failed: " << query.lastError();
            throw std::runtime_error("Failed saving beam measurement points to db");
        }
    }
}

std::vector<double> QARepo::DecodeDoubleArray(QString array) const {
    QString arrayWithoutBraces(array.remove("{").remove("}"));
    QStringList doubleStringList(arrayWithoutBraces.split(",", QString::SkipEmptyParts));
    std::vector<double> doubleVector;
    for (QString val : doubleStringList) {
        doubleVector.push_back(val.toDouble());
    }
    return doubleVector;
}

bool QARepo::CheckConnection() {
    if (disconnect_on_idle_timeout_ > 0) disconnect_timer_.start();
    if (db_->isOpen()) {
        return true;
    } else {
        return this->Connect();
    }
}


