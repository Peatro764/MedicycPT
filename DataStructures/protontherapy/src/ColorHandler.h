#ifndef COLORHANDLER_H
#define COLORHANDLER_H

#include <vector>
#include <QList>
#include <QColor>

namespace colorhandler {
    std::vector<QColor> BaseColors();
    std::vector<QColor> GenerateColors(int n);
    void AddInterpolatedColors(std::vector<QColor>& colors, int n_max);
    QColor InterpolateColor(const QColor &start, const QColor &end, qreal pos);

}

#endif
