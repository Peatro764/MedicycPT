#include "Util.h"

namespace util {
std::vector<QByteArray> Split(QByteArray data, QByteArray separator) {
    std::vector<QByteArray> elements;

    if (separator.isEmpty()) {
        elements.push_back(data);
        return elements;
    } else {

        bool finished = false;
        while (!finished) {
            int index = data.indexOf(separator);
            if (index == -1) {
                elements.push_back(data);
                finished = true;
            } else {
                elements.push_back(data.left(index + separator.size()));
                data = data.right(data.size() - (index + separator.size()));
                if (data.isEmpty()) finished = true;
            }
        }
        return elements;
    }
}
}
