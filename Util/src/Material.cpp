#include "Material.h"

#include <stdexcept>

namespace material {

double ToPlexiglas(double length, MATERIAL material) {
    switch (material) {
    case MATERIAL::PLEXIGLAS:
        return length;
        break;
    case MATERIAL::TISSUE:
        return length / PLEXI2TISSUE;
        break;
    case MATERIAL::WATER:
        return length * WATER2TISSUE / PLEXI2TISSUE;
        break;
    default:
        throw std::runtime_error("Unknown material");
        break;
    }
}

double ToTissue(double length, MATERIAL material) {
    switch (material) {
    case MATERIAL::PLEXIGLAS:
        return length * PLEXI2TISSUE;
        break;
    case MATERIAL::TISSUE:
        return length;
        break;
    case MATERIAL::WATER:
        return length * WATER2TISSUE;
        break;
    default:
        throw std::runtime_error("Unknown material");
        break;
    }
}

double ToWater(double length, MATERIAL material) {
    switch (material) {
    case MATERIAL::PLEXIGLAS:
        return length * PLEXI2TISSUE / WATER2TISSUE;
        break;
    case MATERIAL::TISSUE:
        return length / WATER2TISSUE;
        break;
    case MATERIAL::WATER:
        return length;
        break;
    default:
        throw std::runtime_error("Unknown material");
        break;
    }
}

double Tissue2Plexiglas(const double& l_tissue) {
    return (l_tissue * 1.05 / 1.18);
}

double Plexiglas2Tissue(const double& l_plexiglas) {
    return (l_plexiglas * 1.18 / 1.05);
}

}
