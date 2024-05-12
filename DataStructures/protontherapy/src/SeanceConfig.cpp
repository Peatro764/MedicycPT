#include "SeanceConfig.h"

#include <QDebug>
#include <math.h>
#include <functional>

#include "Conversions.h"
#include "Calc.h"

SeanceConfig::SeanceConfig()
    : init_(false), name_(QString("")), dossier_(0), seance_id_(0), dose_(0.0), debit_(0.0), mu_des_(0), mu_cor_(0), duration_(0), current_stripper_(0)
{}

SeanceConfig::SeanceConfig(QString name,
                           uint32_t dossier,
                           uint16_t seance_id,
                           float dose,
                           float debit,
                           uint32_t mu_des,
                           uint32_t mu_cor,
                           uint32_t duration,
                           uint32_t current_stripper)
    : init_(true), name_(name), dossier_(dossier), seance_id_(seance_id),
      dose_(dose), debit_(debit), mu_des_(mu_des), mu_cor_(mu_cor), duration_(duration), current_stripper_(current_stripper) {
    if (debit_ < 0.0000f) {
        throw std::runtime_error("Cannot create a seance config with near zero or negative debit");
    }
    if (dose_ < 0.0) {
        throw std::runtime_error("Cannot create a seance config with a negative dose");
    }
}

bool SeanceConfig::BelongsTo(const SeancePacket& packet) const {
    return (packet.dossier() == dossier_) && (packet.seance_id() == seance_id_);
}

bool SeanceConfig::BelongsTo(const std::vector<SeancePacket>& seance_packets) const {
    return std::all_of(seance_packets.begin(), seance_packets.end(), [&](SeancePacket s) { return BelongsTo(s); });
}

SeanceRecord SeanceConfig::Convert(QDateTime timestamp, const std::vector<SeancePacket>& seance_packets) const {
    if (!this->BelongsTo(seance_packets)) {
        throw std::runtime_error("Les donnéés recus n'appartiennent pas au config du séance");
    }

    if (seance_packets.empty()) {
        throw std::runtime_error("Les donnéés sont vides.");
    }
    const SeancePacket first_packet = seance_packets.front();
    std::vector<double> duration;
    std::vector<double> i_ch_1;
    std::vector<double> i_ch_2;
    std::vector<int> cf9_status;
    std::vector<int> um_del_1;
    std::vector<int> um_del_2;
    const double i_stripper = static_cast<double>(first_packet.i_stripper());
    const double i_cf9 = static_cast<double>(first_packet.i_cf9());
    for (SeancePacket s : seance_packets) {
        duration.push_back(static_cast<double>(s.duration()) / 1000.0);
        i_ch_1.push_back(static_cast<double>(s.i_ct1()));
        i_ch_2.push_back(static_cast<double>(s.i_ct2()));
        cf9_status.push_back(static_cast<int>(s.cf9_status()));
        if (s.um1() < first_packet.um1()) {
            throw std::runtime_error("UM compteur 1 corrumpu");
        }
        if (s.um2() < first_packet.um2()) {
            throw std::runtime_error("UM compteur 2 corrumpu");
        }
        um_del_1.push_back(static_cast<int>(s.um1() - first_packet.um1()));
        um_del_2.push_back(static_cast<int>(s.um2() - first_packet.um2()));
    }
    return SeanceRecord(timestamp,
                        debit_,
                        duration,
                        i_ch_1, i_ch_2,
                        cf9_status,
                        i_stripper, i_cf9,
                        mu_des_, mu_cor_,
                        um_del_1, um_del_2,
                        0.0);
}

QByteArray SeanceConfig::ByteArray() const {
    QByteArray cmd;
    cmd.append(FormatName(name_));
    cmd.append(conversions::little_endian::AsByteArray(dossier_, 4));
    cmd.append(conversions::little_endian::AsByteArray(seance_id_, 2));
    cmd.append(conversions::little_endian::AsByteArray(dose_));
    cmd.append(conversions::little_endian::AsByteArray(debit_));
    cmd.append(conversions::little_endian::AsByteArray(mu_des_, 4));
    cmd.append(conversions::little_endian::AsByteArray(mu_cor_, 4));
    cmd.append(conversions::little_endian::AsByteArray(duration_, 4));
    cmd.append(conversions::little_endian::AsByteArray(current_stripper_, 4));
    return cmd;
}

SeanceConfig SeanceConfig::UnPack(QByteArray data) {
    if (data.size() != SERIALIZED_PACKET_SIZE) {
        throw std::runtime_error("UploadSeanceConfig command has not the correct size");
    }
    QString name = QString::fromStdString(data.left(20).toStdString());
    const int end_char = name.indexOf('#');
    if (end_char != -1) {
        name = name.left(end_char);
    }
    return SeanceConfig(name,
                        conversions::little_endian::AsUInt32(data.mid(20, 4)),
                        conversions::little_endian::AsUInt16(data.mid(24, 2)),
                        conversions::little_endian::AsFloat(data.mid(26, 4)),
                        conversions::little_endian::AsFloat(data.mid(30, 4)),
                        conversions::little_endian::AsUInt32(data.mid(34, 4)),
                        conversions::little_endian::AsUInt32(data.mid(38, 4)),
                        conversions::little_endian::AsUInt32(data.mid(42, 4)),
                        conversions::little_endian::AsUInt32(data.mid(46, 4)));
}

QByteArray SeanceConfig::FormatName(QString data) const {
    QByteArray formatted = data.toUtf8().left(19).append('#');
    return formatted.append(20 - formatted.size(), ' ');
}

void SeanceConfig::SetMUDes(uint32_t mu_des) {
    mu_des_ = mu_des;
    dose_ = static_cast<float>(calc::UMToDose(static_cast<double>(debit_), static_cast<int>(mu_des)));
}


std::ostream& operator<< (std::ostream &out, const SeanceConfig &seance_config) {
    out << seance_config.name().toStdString() << " " <<
           seance_config.dossier() << " " <<
           seance_config.seance_id() << " " <<
           seance_config.dose() << " " <<
           seance_config.debit() << " " <<
           seance_config.mu_des() << " " <<
           seance_config.mu_cor() << " " <<
           seance_config.duration() << " " <<
           seance_config.current_stripper() << std::endl;
    return out;
}

bool operator==(const SeanceConfig &s1, const SeanceConfig& s2) {
    return (/*s1.name() == s2.name() &&*/
            (std::abs(s1.dose() - s2.dose())) < 0.01f &&
            (std::abs(s1.debit() - s2.debit())) < 0.001f &&
            s1.dossier() == s2.dossier() &&
            s1.duration() == s2.duration() &&
            s1.mu_des() == s2.mu_des() &&
            s1.mu_cor() == s2.mu_cor() &&
            s1.seance_id() == s2.seance_id() &&
            s1.current_stripper() == s2.current_stripper());
}

bool operator!=(const SeanceConfig &s1, const SeanceConfig& s2) {
    return !(s1 == s2);
}

