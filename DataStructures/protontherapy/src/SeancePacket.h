#ifndef PROTONTHERAPY_SEANCEPACKET_H
#define PROTONTHERAPY_SEANCEPACKET_H

#include <QObject>
#include <QByteArray>
#include <iostream>

class SeancePacket {
public:
    const static int SERIALIZED_PACKET_SIZE = 35;
    SeancePacket();
    SeancePacket(uint32_t dossier,
                 uint16_t seance_id,
                 uint8_t cf9_status,
                 uint32_t duration,
                 int32_t i_ct1,
                 int32_t i_ct2,
                 int32_t i_stripper,
                 int32_t i_cf9,
                 uint32_t um1,
                 uint32_t um2);
    QByteArray ByteArray() const;
    static SeancePacket UnPack(QByteArray data);

    bool SameSeance(const SeancePacket& packet) const;
    uint32_t dossier() const { return dossier_; }
    uint16_t seance_id() const { return seance_id_; }
    uint8_t cf9_status() const { return cf9_status_; }
    uint32_t duration() const { return duration_; }
    int32_t i_ct1() const { return i_ct1_; }
    int32_t i_ct2() const { return i_ct2_; }
    int32_t i_stripper() const { return i_stripper_; }
    int32_t i_cf9() const { return i_cf9_; }
    uint32_t um1() const { return um1_; }
    uint32_t um2() const { return um2_; }

private:
    friend bool operator==(const SeancePacket& s1, const SeancePacket& s2);
    friend bool operator!=(const SeancePacket& s1, const SeancePacket& s2);

    uint32_t dossier_;
    uint16_t seance_id_;
    uint8_t cf9_status_;
    uint32_t duration_;
    int32_t i_ct1_;
    int32_t i_ct2_;
    int32_t i_stripper_;
    int32_t i_cf9_;
    uint32_t um1_;
    uint32_t um2_;
};

std::ostream& operator<< (std::ostream &out, const SeancePacket &seance_packet);
bool operator==(const SeancePacket &s1, const SeancePacket& s2);
bool operator!=(const SeancePacket &s1, const SeancePacket& s2);

#endif
