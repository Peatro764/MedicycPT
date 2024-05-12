#include "TestCollimator.h"

#include <iostream>

#include "MillingMachine.h"

void TestCollimator::initTestCase() {}

void TestCollimator::cleanupTestCase() {}

void TestCollimator::init() {}

void TestCollimator::cleanup() {}

void TestCollimator::CreateCollimatorProgram() {
    MillingMachine milling_machine;
    std::vector<Coordinate> coordinates;
    coordinates.push_back(Coordinate(-1.0f, 11.0f));
    coordinates.push_back(Coordinate(2.0f, -12.0f));
    coordinates.push_back(Coordinate(-3.0f, -13.0f));
    coordinates.push_back(Coordinate(4.0f, 14.0f));
    Collimator collimator(12345, Patient(1, "Doe", "John"), coordinates);
    milling_machine.CreateProgram(collimator);
    QString program(milling_machine.GetProgram());

    QString exp_program("");
    QTextStream stream(&exp_program);
    stream.setRealNumberPrecision(3);
    stream << QString("0 BEGIN PGM 12345 MM") << endl;
    stream << "1 BLK FORM 0.1 Z X-17 Y-17 Z-8" << endl;
    stream << "2 BLK FORM 0.2 X+17 Y+17 Z+0" << endl;
    stream << "3 TOOL DEF 1 L+0 R+1,60" << endl;
    stream << "4 TOOL DEF 2 L+0 R+1,50" << endl;
    stream << "5 TOOL DEF 3 L+0 R+1,50" << endl;
    stream << "6 TOOL CALL 1 Z S 3000" << endl;
    stream << "7 L X+0.000 Y+0.000 R0 F MAX M03" << endl;
    stream << "8 L Z+2.000 R0 F MAX M" << endl;
    stream << "9 L Z+0.000 R0 F100 M" << endl;
    stream << "10 LBL 1" << endl;
    stream << "11 L IZ-1.050 R0 F100 M" << endl;
    stream << "12 L X+1.000 Y+11.000 RL F M" << endl;
    stream << "13 L X-2.000 Y-12.000 R F M" << endl;
    stream << "14 L X+3.000 Y-13.000 R F M" << endl;
    stream << "15 L X-4.000 Y+14.000 R F M" << endl;
    stream << "16 L X+0.000 Y+0.000 R0 F M" << endl;
    stream << "17 LBL 0" << endl;
    stream << "18 CALL LBL 1 REP 7 /7" << endl;
    stream << "19 TOOL CALL 2 Z S 3000" << endl;
    stream << "20 L Z-9.000 R0 F MAX M03" << endl;
    stream << "21 CALL LBL 1 REP" << endl;
    stream << "22 TOOL CALL 3 Z S 3000" << endl;
    stream << "23 L Z-9.000 R0 F MAX M03" << endl;
    stream << "24 CALL LBL 1 REP" << endl;
    stream << "25 L Z+10.000 R0 F MAX M" << endl;
    stream << "26 CC X+0.000 Y+0.000" << endl;
    stream << "27 LP PR+22.500 PA+45.000 R0 F MAX M03" << endl;
    stream << "28 L Z+2.000 R0 F MAX M" << endl;
    stream << "29 L Z-2.000 R0 F40 M" << endl;
    stream << "30 LP PR+18.500 PA+45.000 R0 F M" << endl;
    stream << "31 L Z+2.000 R0 F100 M" << endl;
    stream << "32 L R F M25" << endl;
    stream << "33 END PGM 12345 MM" << endl;

    QCOMPARE(program, exp_program);
}

void TestCollimator::EyePlanToMillingProgram() {
    QFile eyeplan_file(":/data/EyePlanCollimator.txt");
    eyeplan_file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream eyeplan_stream(&eyeplan_file);
    Collimator collimator(eyeplan_stream);
    MillingMachine milling_machine;
    milling_machine.CreateProgram(collimator);
    QString milling_program(milling_machine.GetProgram());

    QFile milling_file(":/data/CollimatorMillingProgram.txt");
    milling_file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream milling_stream(&milling_file);
    QString exp_milling_program(milling_stream.readAll());

    QCOMPARE(milling_program, exp_milling_program);
}




