#include "ColorHandler.h"

#include <iterator>

namespace colorhandler {
    std::vector<QColor> GenerateColors(int n) {
        std::vector<QColor> base_colors(BaseColors());
        if (n <= (int)base_colors.size()) {
            return std::vector<QColor>(base_colors.begin(), base_colors.begin() + n);
        }

        std::vector<QColor> colors(base_colors);
        while ((int)colors.size() < n) {
            AddInterpolatedColors(colors, n);
        }
        return std::vector<QColor>(colors.begin(), colors.begin() + n);
    }

    void AddInterpolatedColors(std::vector<QColor>& colors, int n_max) {
        for (auto it = colors.begin() + 1; it < colors.end(); ++it) {
            auto new_color(InterpolateColor(*(it - 1), *it, 0.5));
            it = colors.insert(it, new_color);
            ++it;
            if ((int)colors.size() >= n_max) return;
        }
    }

    QColor InterpolateColor(const QColor &start, const QColor &end, qreal pos) {
        qreal r = start.redF() + ((end.redF() - start.redF()) * pos);
        qreal g = start.greenF() + ((end.greenF() - start.greenF()) * pos);
        qreal b = start.blueF() + ((end.blueF() - start.blueF()) * pos);
        QColor c;
        c.setRgbF(r, g, b);
        return c;
    }

    std::vector<QColor> BaseColors() {
        std::vector<QColor> base_colors;
        base_colors.push_back(QRgb(0x209fdf));
        base_colors.push_back(QRgb(0x99ca53));
        base_colors.push_back(QRgb(0xf6a625));
        //base_colors.push_back(QRgb(0x6d5fd5));
        //base_colors.push_back(QRgb(0xbf593e));
        return base_colors;
    }
}

