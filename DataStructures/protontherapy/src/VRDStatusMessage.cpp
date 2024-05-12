#include "VRDStatusMessage.h"

#include <QDebug>
#include <math.h>

#include "Conversions.h"

VRDStatusMessage::VRDStatusMessage()
    : cf10_closed_(0), ct_current_(0.0), ct_frequency_(0.0), ct_charge_per_pulse_(0.0), mu_(0),
      em2_current_(0), em2_frequency_(0), em2_charge_per_pulse_(0)
{}

VRDStatusMessage::VRDStatusMessage(short cf10_closed,
                                   float ct_current,
                                   float ct_frequency,
                                   float ct_charge_per_pulse,
                                   uint32_t mu,
                                   float em2_current,
                                   float em2_frequency,
                                   float em2_charge_per_pulse)
    : cf10_closed_(cf10_closed), ct_current_(ct_current), ct_frequency_(ct_frequency),
      ct_charge_per_pulse_(ct_charge_per_pulse), mu_(mu), em2_current_(em2_current), em2_frequency_(em2_frequency),
      em2_charge_per_pulse_(em2_charge_per_pulse)
{}

QByteArray VRDStatusMessage::ByteArray() const {
    QByteArray cmd;
    cmd.append(conversions::little_endian::AsByteArray(cf10_closed_, 2));
    cmd.append(conversions::little_endian::AsByteArray(ct_current_));
    cmd.append(conversions::little_endian::AsByteArray(ct_frequency_));
    cmd.append(conversions::little_endian::AsByteArray(ct_charge_per_pulse_));
    cmd.append(conversions::little_endian::AsByteArray(mu_, 4));
    cmd.append(conversions::little_endian::AsByteArray(em2_current_));
    cmd.append(conversions::little_endian::AsByteArray(em2_frequency_));
    cmd.append(conversions::little_endian::AsByteArray(em2_charge_per_pulse_));
    return cmd;
}

QString VRDStatusMessage::ToString() const {
    QString str;
    str.append("CF10 closed ");
    str.append(cf10_closed_).append("\n");
    str.append("CT current ");
    str.append(QString::number(ct_current_)).append("\n");
    str.append("CT frequency ");
    str.append(QString::number(ct_frequency_)).append("\n");
    str.append("CT charge per pulse ");
    str.append(QString::number(ct_charge_per_pulse_)).append("\n");
    str.append("MU ");
    str.append(QString::number(mu_)).append("\n");
    str.append("EM2 current ");
    str.append(QString::number(em2_current_)).append("\n");
    str.append("EM2 frequency ");
    str.append(QString::number(em2_frequency_)).append("\n");
    str.append("EM2 charge per pulse ");
    str.append(QString::number(em2_charge_per_pulse_)).append("\n");

    return str;
}


VRDStatusMessage VRDStatusMessage::UnPack(QByteArray data) {
    if (data.size() != SERIALIZED_PACKET_SIZE) {
        throw std::runtime_error("VRDStatusMessage has not the correct size");
    }
    return VRDStatusMessage(static_cast<short>(conversions::little_endian::AsUInt16(data.left(2))), // cf10_closed
                        conversions::little_endian::AsFloat(data.mid(2, 4)), // ct_current
                        conversions::little_endian::AsFloat(data.mid(6, 4)), // ct_frequency
                        conversions::little_endian::AsFloat(data.mid(10, 4)), // ct_current
                        conversions::little_endian::AsUInt32(data.mid(14, 4)), // mu
                        conversions::little_endian::AsFloat(data.mid(18, 4)), // em2_current
                        conversions::little_endian::AsFloat(data.mid(22, 4)), // em2_frequency
                        conversions::little_endian::AsFloat(data.mid(26, 4))); // em2_current
}


std::ostream& operator<< (std::ostream &out, const VRDStatusMessage &status_message) {
    out << status_message.cf10_closed() << " " <<
           status_message.ct_current() << " " <<
           status_message.ct_frequency() << " " <<
           status_message.ct_charge_per_pulse() << " " <<
           status_message.mu() << " " <<
           status_message.em2_current() << " " <<
           status_message.em2_frequency() << " " <<
           status_message.em2_charge_per_pulse() << std::endl;
    return out;
}

bool operator==(const VRDStatusMessage &s1, const VRDStatusMessage& s2) {
    return (s1.cf10_closed() == s2.cf10_closed() &&
            (s1.ct_current() - s2.ct_current())/s1.ct_current() < 0.0001  &&
            (s1.ct_frequency() - s2.ct_frequency())/s1.ct_frequency() < 0.0001  &&
            (s1.ct_charge_per_pulse() - s2.ct_charge_per_pulse())/s1.ct_charge_per_pulse() < 0.0001  &&
            (s1.mu() == s2.mu()) &&
            (s1.em2_current() - s2.em2_current())/s1.em2_current() < 0.0001  &&
            (s1.em2_frequency() - s2.em2_frequency())/s1.em2_frequency() < 0.0001  &&
            (s1.em2_charge_per_pulse() - s2.em2_charge_per_pulse())/s1.em2_charge_per_pulse() < 0.0001);
}

bool operator!=(const VRDStatusMessage &s1, const VRDStatusMessage& s2) {
    return !(s1 == s2);
}

