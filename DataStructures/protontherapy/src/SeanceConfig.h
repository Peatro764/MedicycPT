#ifndef PROTONTHERAPY_SEANCECONFIG_H
#define PROTONTHERAPY_SEANCECONFIG_H

#include <QObject>
#include <QByteArray>
#include <iostream>

#include "SeancePacket.h"
#include "SeanceRecord.h"

class SeanceConfig {
public:
    const static int SERIALIZED_PACKET_SIZE = 50;
    SeanceConfig();
    SeanceConfig(QString name,
                 uint32_t dossier,
                 uint16_t seance_id,
                 float dose,
                 float debit,
                 uint32_t mu_des,
                 uint32_t mu_cor,
                 uint32_t duration,
                 uint32_t current_stripper);
    QByteArray ByteArray() const;
    static SeanceConfig UnPack(QByteArray data);

    bool Initialised() const { return init_; }
    bool BelongsTo(const SeancePacket& seance_packets) const;
    bool BelongsTo(const std::vector<SeancePacket>& seance_packets) const;
    SeanceRecord Convert(QDateTime timestamp, const std::vector<SeancePacket>& seance_packets) const;
    QString name() const { return name_; }
    uint32_t dossier() const { return dossier_; }
    uint16_t seance_id() const { return seance_id_; }
    float dose() const { return dose_; }
    float debit() const { return debit_; }
    uint32_t mu_des() const { return mu_des_; }
    uint32_t mu_cor() const { return mu_cor_; }
    uint32_t duration() const { return duration_; }
    uint32_t current_stripper() const { return current_stripper_; }

    void SetMUDes(uint32_t mu_des);
    void SetMUCor(uint32_t mu_cor) { mu_cor_ = mu_cor; }
    void SetDuration(uint32_t duration) { duration_ = duration; }
    void SetCurrentStripper(uint32_t current_stripper) { current_stripper_ = current_stripper; }

private:
    QByteArray FormatName(QString data) const;
    friend bool operator==(const SeanceConfig& s1, const SeanceConfig& s2);
    friend bool operator!=(const SeanceConfig& s1, const SeanceConfig& s2);

    bool init_;
    QString name_;
    uint32_t dossier_;
    uint16_t seance_id_;
    float dose_;
    float debit_;
    uint32_t mu_des_;
    uint32_t mu_cor_;
    uint32_t duration_;
    uint32_t current_stripper_;
};

std::ostream& operator<< (std::ostream &out, const SeanceConfig &seance_config);
bool operator==(const SeanceConfig &s1, const SeanceConfig& s2);
bool operator!=(const SeanceConfig &s1, const SeanceConfig& s2);

#endif
