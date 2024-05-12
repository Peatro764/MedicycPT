#include "TestCuveCube.h"

#include "qcustomplot.h"

#include "Calc.h"

void TestCuveCube::initTestCase() {}

void TestCuveCube::cleanupTestCase() {}

void TestCuveCube::init() {}

void TestCuveCube::cleanup() {}

void TestCuveCube::getters() {
    QDateTime timestamp = QDateTime::currentDateTime();
    QString comment("hej");
    double noise = 0.345;
    std::map<Axis, AxisConfig> configs;
    configs[Axis::X] = AxisConfig(2, 0.3, 0.1);
    configs[Axis::Y] = AxisConfig(3, 0.4, 0.2);
    configs[Axis::Z] = AxisConfig(4, 0.5, 0.3);
    CuveCube cube(timestamp, comment, configs, std::vector<MeasurementPoint>(), noise);

    QCOMPARE(cube.timestamp(), timestamp);
    QCOMPARE(cube.comment(), comment);
    QCOMPARE(cube.GetAxisConfig(Axis::X), configs.at(Axis::X));
    QCOMPARE(cube.GetAxisConfig(Axis::Y), configs.at(Axis::Y));
    QCOMPARE(cube.GetAxisConfig(Axis::Z), configs.at(Axis::Z));
    QVERIFY(calc::AlmostEqual(noise, cube.GetNoise(), 0.0001));
}

void TestCuveCube::configs() {
    std::map<Axis, AxisConfig> configs;
    configs[Axis::X] = AxisConfig(2, 0.3, 0.1);
    configs[Axis::Y] = AxisConfig(3, 0.4, 0.2);
    configs[Axis::Z] = AxisConfig(4, 0.5, 0.3);

    CuveCube cube(configs, 0.0);

    QCOMPARE(cube.nbins(Axis::X), 2);
    QCOMPARE(cube.nbins(Axis::Y), 3);
    QCOMPARE(cube.nbins(Axis::Z), 4);

    QVERIFY(calc::AlmostEqual(cube.pos(Axis::X, 0), 0.3, 0.0001));
    QVERIFY(calc::AlmostEqual(cube.pos(Axis::X, 1), 0.4, 0.0001)); // t

    QVERIFY(calc::AlmostEqual(cube.pos(Axis::Y, 0), 0.4, 0.0001));
    QVERIFY(calc::AlmostEqual(cube.pos(Axis::Y, 1), 0.6, 0.0001));
    QVERIFY(calc::AlmostEqual(cube.pos(Axis::Y, 2), 0.8, 0.0001));

    QVERIFY(calc::AlmostEqual(cube.pos(Axis::Z, 0), 0.5, 0.0001));
    QVERIFY(calc::AlmostEqual(cube.pos(Axis::Z, 1), 0.8, 0.0001));
    QVERIFY(calc::AlmostEqual(cube.pos(Axis::Z, 2), 1.1, 0.0001));
    QVERIFY(calc::AlmostEqual(cube.pos(Axis::Z, 3), 1.4, 0.0001));

    try {
        cube.pos(Axis::X, 2);
        QFAIL("TestCuveCube::getters Should have thrown 1");
    }
    catch (std::exception& exc) {}

    try {
        cube.pos(Axis::Y, 3);
        QFAIL("TestCuveCube::getters Should have thrown 2");
    }
    catch (std::exception& exc) {}

    try {
        cube.pos(Axis::Z, 5);
        QFAIL("TestCuveCube::getters Should have thrown 3");
    }
    catch (std::exception& exc) {}

}

void TestCuveCube::points() {
    // DIFFERENT CONFIG
    std::map<Axis, AxisConfig> configs1;
    configs1[Axis::X] = AxisConfig(2, 0.3, 0.1);
    configs1[Axis::Y] = AxisConfig(3, 0.4, 0.2);
    configs1[Axis::Z] = AxisConfig(4, 0.5, 0.3);

    std::map<Axis, AxisConfig> configs2;
    configs2[Axis::X] = AxisConfig(2, 0.3, 0.1);
    configs2[Axis::Y] = AxisConfig(3, 0.4, 0.2);
    configs2[Axis::Z] = AxisConfig(4, 0.5, 0.4);

    QDateTime timestamp1 = QDateTime::currentDateTime();
    CuveCube cube1(timestamp1, QString("SSS"), configs1, std::vector<MeasurementPoint>(), 0.0);
    CuveCube cube2(timestamp1, QString("SSS"), configs2, std::vector<MeasurementPoint>(), 0.0);
    QVERIFY(cube1 == cube1);
    QVERIFY(cube1 != cube2);

    // DIFFERENT POINTS
    std::vector<MeasurementPoint> points1;
    points1.push_back(MeasurementPoint(Point(0.3, 0.4, 0.5), BeamSignal(1, 2)));
    std::vector<MeasurementPoint> points2;
    points2.push_back(MeasurementPoint(Point(0.3, 0.4, 0.5), BeamSignal(1, 1)));

    CuveCube cube3(timestamp1, QString("SSS"), configs1, points1, 0.0);
    CuveCube cube4(timestamp1, QString("SSS"), configs1, points2, 0.0);
    QVERIFY(cube3 != cube4);

    // DIFFERENT TIMESTAMPS
    QDateTime timestamp2 = timestamp1.addDays(1);
    CuveCube cube5(timestamp1, QString("SSS"), configs1, std::vector<MeasurementPoint>(), 0.0);
    CuveCube cube6(timestamp2, QString("SSS"), configs1, std::vector<MeasurementPoint>(), 0.0);
    QVERIFY(cube5 != cube6);

    // IDENTICAL MAPS
    std::map<Axis, AxisConfig> configs;
    configs[Axis::X] = AxisConfig(2, 0.3, 0.1);
    configs[Axis::Y] = AxisConfig(3, 0.4, 0.2);
    configs[Axis::Z] = AxisConfig(4, 0.5, 0.3);

    std::vector<MeasurementPoint> points;
    points.push_back(MeasurementPoint(Point(0.3, 0.4, 0.5), BeamSignal(1, 1)));
    points.push_back(MeasurementPoint(Point(0.351, 0.4, 0.5), BeamSignal(2, 1)));
    points.push_back(MeasurementPoint(Point(0.3, 0.6, 0.5), BeamSignal(4, 1)));
    points.push_back(MeasurementPoint(Point(0.3, 0.8, 0.5), BeamSignal(6, 1)));
    points.push_back(MeasurementPoint(Point(0.3, 0.4, 0.6), BeamSignal(7, 1)));
    points.push_back(MeasurementPoint(Point(0.3, 0.4, 1.1), BeamSignal(8, 1)));
    points.push_back(MeasurementPoint(Point(0.37, 0.77, 1.4), BeamSignal(9, 1)));
    CuveCube cube(QDateTime::currentDateTime(), QString("SSS"), configs, points, 0.0);

    std::vector<MeasurementPoint> exp_points;
    exp_points.push_back(MeasurementPoint(Point(0.3, 0.4, 0.5), BeamSignal(7, 1)));
    exp_points.push_back(MeasurementPoint(Point(0.3, 0.4, 0.8), BeamSignal(0, 0)));
    exp_points.push_back(MeasurementPoint(Point(0.3, 0.4, 1.1), BeamSignal(8, 1)));
    exp_points.push_back(MeasurementPoint(Point(0.3, 0.4, 1.4), BeamSignal(0, 0)));

    exp_points.push_back(MeasurementPoint(Point(0.3, 0.6, 0.5), BeamSignal(4, 1)));
    exp_points.push_back(MeasurementPoint(Point(0.3, 0.6, 0.8), BeamSignal(0, 0)));
    exp_points.push_back(MeasurementPoint(Point(0.3, 0.6, 1.1), BeamSignal(0, 0)));
    exp_points.push_back(MeasurementPoint(Point(0.3, 0.6, 1.4), BeamSignal(0, 0)));

    exp_points.push_back(MeasurementPoint(Point(0.3, 0.8, 0.5), BeamSignal(6, 1)));
    exp_points.push_back(MeasurementPoint(Point(0.3, 0.8, 0.8), BeamSignal(0, 0)));
    exp_points.push_back(MeasurementPoint(Point(0.3, 0.8, 1.1), BeamSignal(0, 0)));
    exp_points.push_back(MeasurementPoint(Point(0.3, 0.8, 1.4), BeamSignal(0, 0)));

    exp_points.push_back(MeasurementPoint(Point(0.4, 0.4, 0.5), BeamSignal(2, 1)));
    exp_points.push_back(MeasurementPoint(Point(0.4, 0.4, 0.8), BeamSignal(0, 0)));
    exp_points.push_back(MeasurementPoint(Point(0.4, 0.4, 1.1), BeamSignal(0, 0)));
    exp_points.push_back(MeasurementPoint(Point(0.4, 0.4, 1.4), BeamSignal(0, 0)));

    exp_points.push_back(MeasurementPoint(Point(0.4, 0.6, 0.5), BeamSignal(0, 0)));
    exp_points.push_back(MeasurementPoint(Point(0.4, 0.6, 0.8), BeamSignal(0, 0)));
    exp_points.push_back(MeasurementPoint(Point(0.4, 0.6, 1.1), BeamSignal(0, 0)));
    exp_points.push_back(MeasurementPoint(Point(0.4, 0.6, 1.4), BeamSignal(0, 0)));

    exp_points.push_back(MeasurementPoint(Point(0.4, 0.8, 0.5), BeamSignal(0, 0)));
    exp_points.push_back(MeasurementPoint(Point(0.4, 0.8, 0.8), BeamSignal(0, 0)));
    exp_points.push_back(MeasurementPoint(Point(0.4, 0.8, 1.1), BeamSignal(0, 0)));
    exp_points.push_back(MeasurementPoint(Point(0.4, 0.8, 1.4), BeamSignal(9, 1)));

    std::vector<MeasurementPoint> from_cube = cube.GetMeasurementPoints();

    QCOMPARE(exp_points, from_cube);
}

void TestCuveCube::colormap_dimensions() {
    std::map<Axis, AxisConfig> configs;
    configs[Axis::X] = AxisConfig(2, 0.0, 0.1);
    configs[Axis::Y] = AxisConfig(3, 1.0, 0.2);
    configs[Axis::Z] = AxisConfig(4, -1.0, 0.3);

    CuveCube cube(configs, 0.0);

    QCPColorMapData map_xy = cube.GetColorMap(PROJECTION::XY, 0);
    QCOMPARE(map_xy.keySize(), 2);
    QCOMPARE(map_xy.valueSize(), 3);
    QCOMPARE(map_xy.keyRange(), QCPRange(0.0, 0.1));
    QCOMPARE(map_xy.valueRange(), QCPRange(1.0, 1.4));

    cube.GetColorMap(PROJECTION::XY, 1);
    cube.GetColorMap(PROJECTION::XY, 2);
    cube.GetColorMap(PROJECTION::XY, 3);
    try {
        cube.GetColorMap(PROJECTION::XY, 4);
        QFAIL("TestCuveCube::colormap_dimensions should have thrown 1");
    }
    catch (std::exception& exc) {}

    QCPColorMapData map_zx = cube.GetColorMap(PROJECTION::ZX, 0);
    QCOMPARE(map_zx.keySize(), 4);
    QCOMPARE(map_zx.valueSize(), 2);
    QVERIFY(calc::AlmostEqual(map_zx.keyRange().lower, -1.0, 0.001));
    QVERIFY(calc::AlmostEqual(map_zx.keyRange().upper, -0.1, 0.001));
    QCOMPARE(map_zx.valueRange(), QCPRange(0.0, 0.1));

    cube.GetColorMap(PROJECTION::ZX, 1);
    cube.GetColorMap(PROJECTION::ZX, 2);
    try {
        cube.GetColorMap(PROJECTION::ZX, 3);
        QFAIL("TestCuveCube::colormap_dimensions should have thrown 2");
    }
    catch (std::exception& exc) {}


    QCPColorMapData map_yz = cube.GetColorMap(PROJECTION::YZ, 0);
    QCOMPARE(map_yz.keySize(), 3);
    QCOMPARE(map_yz.valueSize(), 4);
    QCOMPARE(map_yz.keyRange(), QCPRange(1.0, 1.4));
    QVERIFY(calc::AlmostEqual(map_yz.valueRange().lower, -1.0, 0.001));
    QVERIFY(calc::AlmostEqual(map_yz.valueRange().upper, -0.1, 0.001));

    cube.GetColorMap(PROJECTION::YZ, 1);
    try {
        cube.GetColorMap(PROJECTION::YZ, 2);
        QFAIL("TestCuveCube::colormap_dimensions should have thrown 3");
    }
    catch (std::exception& exc) {}
}

void TestCuveCube::colormap_content() {
    std::map<Axis, AxisConfig> configs;
    configs[Axis::X] = AxisConfig(2, 0.0, 0.1);
    configs[Axis::Y] = AxisConfig(3, 1.0, 0.2);
    configs[Axis::Z] = AxisConfig(4, -1.0, 0.3);

    CuveCube cube(configs, 0.0);

    // empty map
    QCPColorMapData map_empty = cube.GetColorMap(PROJECTION::XY, 0);
    for (int k = 0; k < map_empty.keySize(); ++k) {
        for (int v = 0; v < map_empty.valueSize(); ++v) {
            QVERIFY(calc::AlmostEqual(map_empty.cell(k, v), 0.0, 0.0001));
        }
    }

    // z slice 0
    cube.Add(0, 1, 0, BeamSignal(1.0, 1.0));
    cube.Add(1, 2, 0, BeamSignal(2.0, 1.0));

    // z slice 2
    cube.Add(0, 1, 2, BeamSignal(10.0, 1.0));
    cube.Add(1, 2, 2, BeamSignal(20.0, 1.0));

    QCPColorMapData map_xy_1 = cube.GetColorMap(PROJECTION::XY, 0);
    QVERIFY(calc::AlmostEqual(map_xy_1.cell(0, 1), 1.0, 0.0001));
    QVERIFY(calc::AlmostEqual(map_xy_1.cell(1, 2), 2.0, 0.0001));

    QCPColorMapData map_xy_2 = cube.GetColorMap(PROJECTION::XY, 2);
    QVERIFY(calc::AlmostEqual(map_xy_2.cell(0, 1), 10.0, 0.0001));
    QVERIFY(calc::AlmostEqual(map_xy_2.cell(1, 2), 20.0, 0.0001));

    QCPColorMapData map_zx_1 = cube.GetColorMap(PROJECTION::ZX, 1);
    QVERIFY(calc::AlmostEqual(map_zx_1.cell(0, 0), 1.0, 0.0001));
    QVERIFY(calc::AlmostEqual(map_zx_1.cell(2, 0), 10.0, 0.0001));

    QCPColorMapData map_zx_2 = cube.GetColorMap(PROJECTION::ZX, 2);
    QVERIFY(calc::AlmostEqual(map_zx_2.cell(0, 1), 2.0, 0.0001));
    QVERIFY(calc::AlmostEqual(map_zx_2.cell(2, 1), 20.0, 0.0001));

    QCPColorMapData map_yz_1 = cube.GetColorMap(PROJECTION::YZ, 0);
    QVERIFY(calc::AlmostEqual(map_yz_1.cell(1, 0), 1.0, 0.0001));
    QVERIFY(calc::AlmostEqual(map_yz_1.cell(1, 2), 10.0, 0.0001));

    QCPColorMapData map_yz_2 = cube.GetColorMap(PROJECTION::YZ, 1);
    QVERIFY(calc::AlmostEqual(map_yz_2.cell(2, 0), 2.0, 0.0001));
    QVERIFY(calc::AlmostEqual(map_yz_2.cell(2, 2), 20.0, 0.0001));
}
