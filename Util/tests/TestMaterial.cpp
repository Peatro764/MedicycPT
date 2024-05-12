#include "TestMaterial.h"

#include <iostream>
#include <QDateTime>

#include "Material.h"
#include "Calc.h"

void TestMaterial::initTestCase() {
    maxRelDiff_ = 0.001;
}

void TestMaterial::cleanupTestCase() {}

void TestMaterial::init() {}

void TestMaterial::cleanup() {}

void TestMaterial::TestPlexiToTissue() {
     QVERIFY(calc::AlmostEqual(material::Plexiglas2Tissue(1.0), 1.12381, 0.0001));
     QVERIFY(calc::AlmostEqual(material::Tissue2Plexiglas(1.0), 0.88983, 0.0001));
}

void TestMaterial::TestToPlexiglas() {
    qDebug() << "PLEXI -> PLEXI " << material::ToPlexiglas(1.0, material::MATERIAL::PLEXIGLAS);
    qDebug() << "TISSUE -> PLEXI " << material::ToPlexiglas(1.0, material::MATERIAL::TISSUE);
    qDebug() << "WATER -> PLEXI " << material::ToPlexiglas(1.0, material::MATERIAL::WATER);
    QVERIFY(calc::AlmostEqual(material::ToPlexiglas(1.0, material::MATERIAL::PLEXIGLAS), 1.0, 0.0001));
    QVERIFY(calc::AlmostEqual(material::ToPlexiglas(1.0, material::MATERIAL::TISSUE), 1.0 / 1.106, 0.0001));
    QVERIFY(calc::AlmostEqual(material::ToPlexiglas(1.0, material::MATERIAL::WATER), 0.961739 / 1.106, 0.0001));
}

void TestMaterial::TestToWater() {
    qDebug() << "PLEXI -> WATER " << material::ToWater(1.0, material::MATERIAL::PLEXIGLAS);
    qDebug() << "TISSUE -> WATER " << material::ToWater(1.0, material::MATERIAL::TISSUE);
    qDebug() << "WATER -> WATER " << material::ToWater(1.0, material::MATERIAL::WATER);
    QVERIFY(calc::AlmostEqual(material::ToWater(1.0, material::MATERIAL::PLEXIGLAS), 1.106 / 0.961739, 0.0001));
    QVERIFY(calc::AlmostEqual(material::ToWater(1.0, material::MATERIAL::TISSUE), 1.0 / 0.961739, 0.0001));
    QVERIFY(calc::AlmostEqual(material::ToWater(1.0, material::MATERIAL::WATER), 1.0, 0.0001));
}

void TestMaterial::TestToTissue() {
    qDebug() << "PLEXI -> Tissue " << material::ToTissue(1.0, material::MATERIAL::PLEXIGLAS);
    qDebug() << "TISSUE -> Tissue " << material::ToTissue(1.0, material::MATERIAL::TISSUE);
    qDebug() << "WATER -> Tisue " << material::ToTissue(1.0, material::MATERIAL::WATER);
    QVERIFY(calc::AlmostEqual(material::ToTissue(1.0, material::MATERIAL::PLEXIGLAS), 1.106, 0.0001));
    QVERIFY(calc::AlmostEqual(material::ToTissue(1.0, material::MATERIAL::TISSUE), 1.000, 0.0001));
    QVERIFY(calc::AlmostEqual(material::ToTissue(1.0, material::MATERIAL::WATER), 0.961739, 0.0001));
}


