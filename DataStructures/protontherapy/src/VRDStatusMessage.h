#ifndef PROTONTHERAPY_VRDSTATUSMESSAGE_H
#define PROTONTHERAPY_VRDSTATUSMESSAGE_H

#include <QObject>
#include <QByteArray>
#include <iostream>

class VRDStatusMessage {
public:
    const static int SERIALIZED_PACKET_SIZE = 30; // + headers 6
    VRDStatusMessage();
    VRDStatusMessage(short cf10_closed,
                     float ct_current,
                     float ct_frequency,
                     float ct_charge_per_pulse,
                     uint32_t mu,
                     float em2_current,
                     float em2_frequency,
                     float em2_charge_per_pulse);
    QByteArray ByteArray() const;
    static VRDStatusMessage UnPack(QByteArray data);
    QString ToString() const;

    short cf10_closed() const { return cf10_closed_; }
    float ct_current() const { return ct_current_; }
    float ct_frequency() const { return ct_frequency_; }
    float ct_charge_per_pulse() const { return ct_charge_per_pulse_; }
    uint32_t mu() const { return mu_; }
    float em2_current() const { return em2_current_; }
    float em2_frequency() const { return em2_frequency_; }
    float em2_charge_per_pulse() const { return em2_charge_per_pulse_; }

private:
    friend bool operator==(const VRDStatusMessage& s1, const VRDStatusMessage& s2);
    friend bool operator!=(const VRDStatusMessage& s1, const VRDStatusMessage& s2);

    short cf10_closed_;
    float ct_current_;
    float ct_frequency_;
    float ct_charge_per_pulse_;
    uint32_t mu_;
    float em2_current_;
    float em2_frequency_;
    float em2_charge_per_pulse_;
};

std::ostream& operator<< (std::ostream &out, const VRDStatusMessage &status_message);
bool operator==(const VRDStatusMessage &s1, const VRDStatusMessage& s2);
bool operator!=(const VRDStatusMessage &s1, const VRDStatusMessage& s2);

#endif
