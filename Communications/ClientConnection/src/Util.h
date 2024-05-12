#ifndef CLIENTCONNECTION_UTIL_H
#define CLIENTCONNECTION_UTIL_H

#include <vector>
#include <QByteArray>

namespace util {
    std::vector<QByteArray> Split(QByteArray data, QByteArray separator);
}

#endif
