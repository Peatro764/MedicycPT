#include "Conversions.h"

#include <QDebug>
#include <math.h>

namespace conversions {

namespace little_endian {

QByteArray AsByteArray(int32_t value, int n_bytes) {
    union {
        uint32_t ns;
        int32_t  s;
     } un;
     un.s = value;
     return AsByteArray(un.ns, n_bytes);
}

    QByteArray AsByteArray(uint32_t value, int n_bytes) {
        // LS BYTE -> MS BYTE
        // Thus, "1" is represented as "100"
        if (value > (std::pow(2, 8 * n_bytes) - 1)) {
            QString message = QString("The unsigned value ") + QString::number(value) + " cannot be represented in " +QString::number(n_bytes) + " bytes";
            throw std::runtime_error(message.toStdString().c_str());
        }
        QByteArray ba;
        ba.resize(n_bytes);
        for (int byte = 0; byte < n_bytes; ++byte) {
            ba[byte] = static_cast<char>((value >> 8*byte) & 0xFF);
        }
        return ba;
    }

    QByteArray AsByteArray(float value) {
        union {
            uint32_t i;
            float    f;
         } un;
         un.f = value;
         return AsByteArray(un.i, 4);
    }

    uint32_t AsUInt32(QByteArray data) {
        const int size = data.size();
        if (size > 4) { throw std::runtime_error("AsUInt32 only converts bytearrays with a size up to 4 bytes"); }
        uint32_t value(0);
        for (int byte = 0; byte < size; ++byte) {
            value = value + static_cast<uint32_t>((static_cast<uint8_t>(data[byte]) << 8*byte));
        }
        return value;
    }

    int32_t AsInt32(QByteArray data) {
        uint32_t value = AsUInt32(data);
        union {
            uint32_t ns;
            int32_t  s;
         } un;
         un.ns = value;
        return un.s;
    }

    uint16_t AsUInt16(QByteArray data) {
        const int size = data.size();
        if (size > 2) { throw std::runtime_error("AsUInt16 only converts bytearrays with a size up to 2 bytes"); }
        uint16_t value(0);
        for (int byte = 0; byte < size; ++byte) {
            value = value + static_cast<uint16_t>((static_cast<uint8_t>(data[byte]) << 8*byte));
        }
        return value;
    }

    uint8_t AsUInt8(QByteArray data) {
        const int size = data.size();
        if (size > 1) { throw std::runtime_error("AsUInt8 only converts bytearrays with a size up to 1 bytes"); }
        uint8_t value(0);
        for (int byte = 0; byte < size; ++byte) {
            value = value + static_cast<uint8_t>((static_cast<uint8_t>(data[byte]) << 8*byte));
        }
        return value;
    }

    float AsFloat(QByteArray data) {
        const int size = data.size();
        if (size != 4) { throw std::runtime_error("AsFloat only converts bytearrays with a size of 4 bytes"); }
        const uint32_t nmb = AsUInt32(data);
        union {
            uint32_t i;
            float    f;
         } value;
         value.i = nmb;
         return value.f;
    }
}
}

