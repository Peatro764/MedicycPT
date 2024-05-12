#ifndef UTIL_CONVERSIONS_H
#define UTIL_CONVERSIONS_H

#include <QObject>
#include <QByteArray>

namespace conversions {

namespace little_endian {
    QByteArray AsByteArray(int32_t value, int n_bytes);
    QByteArray AsByteArray(uint32_t value, int n_bytes);
    QByteArray AsByteArray(float value);

    int32_t AsInt32(QByteArray data);
    uint32_t AsUInt32(QByteArray data);
    uint16_t AsUInt16(QByteArray data);
    uint8_t AsUInt8(QByteArray data);
    float AsFloat(QByteArray data);
}

}
#endif
