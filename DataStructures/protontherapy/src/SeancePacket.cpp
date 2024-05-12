#include "SeancePacket.h"

#include <QDebug>
#include <math.h>

#include "Conversions.h"

SeancePacket::SeancePacket()
    : dossier_(0), seance_id_(0), cf9_status_(0), duration_(0), i_ct1_(0),
      i_ct2_(0), i_stripper_(0), i_cf9_(0), um1_(0), um2_(0)
{}

SeancePacket::SeancePacket(uint32_t dossier,
                           uint16_t seance_id,
                           uint8_t cf9_status,
                           uint32_t duration,
                           int32_t i_ct1,
                           int32_t i_ct2,
                           int32_t i_stripper,
                           int32_t i_cf9,
                           uint32_t um1,
                           uint32_t um2)
    : dossier_(dossier), seance_id_(seance_id), cf9_status_(cf9_status), duration_(duration), i_ct1_(i_ct1),
      i_ct2_(i_ct2), i_stripper_(i_stripper), i_cf9_(i_cf9), um1_(um1), um2_(um2)
{}

QByteArray SeancePacket::ByteArray() const {
    QByteArray cmd;
    cmd.append(conversions::little_endian::AsByteArray(dossier_, 4));
    cmd.append(conversions::little_endian::AsByteArray(seance_id_, 2));
    cmd.append(conversions::little_endian::AsByteArray(cf9_status_, 1));
    cmd.append(conversions::little_endian::AsByteArray(duration_, 4));
    cmd.append(conversions::little_endian::AsByteArray(i_ct1_, 4));
    cmd.append(conversions::little_endian::AsByteArray(i_ct2_, 4));
    cmd.append(conversions::little_endian::AsByteArray(i_stripper_, 4));
    cmd.append(conversions::little_endian::AsByteArray(i_cf9_, 4));
    cmd.append(conversions::little_endian::AsByteArray(um1_, 4));
    cmd.append(conversions::little_endian::AsByteArray(um2_, 4));
    return cmd;
}

SeancePacket SeancePacket::UnPack(QByteArray data) {
    if (data.size() != SERIALIZED_PACKET_SIZE) {
        throw std::runtime_error("SeancePacket has not the correct size");
    }
    return SeancePacket(conversions::little_endian::AsUInt32(data.left(4)), // dossier
                        conversions::little_endian::AsUInt16(data.mid(4, 2)), // seance_id
                        conversions::little_endian::AsUInt8(data.mid(6, 1)), // cf9_status
                        conversions::little_endian::AsUInt32(data.mid(7, 4)),  // duration
                        conversions::little_endian::AsInt32(data.mid(11, 4)),  // i_ct1
                        conversions::little_endian::AsInt32(data.mid(15, 4)),  // i_ct2
                        conversions::little_endian::AsInt32(data.mid(19, 4)),  // i_stripper
                        conversions::little_endian::AsInt32(data.mid(23, 4)),  // i_cf9
                        conversions::little_endian::AsUInt32(data.mid(27, 4)),  // um1
                        conversions::little_endian::AsUInt32(data.mid(31, 4)));  // um2
}

bool SeancePacket::SameSeance(const SeancePacket& packet) const {
    return (packet.dossier() == this->dossier() &&
            packet.seance_id() == this->seance_id());
}

std::ostream& operator<< (std::ostream &out, const SeancePacket &seance_packet) {
    out << seance_packet.dossier() << " " <<
           seance_packet.seance_id() << " " <<
           seance_packet.cf9_status() << " " <<
           seance_packet.duration() << " " <<
           seance_packet.i_ct1() << " " <<
           seance_packet.i_ct2() << " " <<
           seance_packet.i_stripper() << " " <<
           seance_packet.i_cf9() << " " <<
           seance_packet.um1() << " " <<
           seance_packet.um2() << std::endl;
    return out;
}

bool operator==(const SeancePacket &s1, const SeancePacket& s2) {
    return (s1.dossier() == s2.dossier() &&
            s1.seance_id() == s2.seance_id() &&
            s1.cf9_status() == s2.cf9_status() &&
            s1.duration() == s2.duration() &&
            s1.i_ct1() == s2.i_ct1() &&
            s1.i_ct2() == s2.i_ct2() &&
            s1.i_cf9() == s2.i_cf9() &&
            s1.i_stripper() == s2.i_stripper() &&
            s1.um1() == s2.um1() &&
            s1.um2() == s2.um2());
}

bool operator!=(const SeancePacket &s1, const SeancePacket& s2) {
    return !(s1 == s2);
}

