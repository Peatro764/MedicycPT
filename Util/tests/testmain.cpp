#include <QtTest>
#include "TestCalc.h"
#include "TestMaterial.h"
#include "TestConversions.h"

using namespace conversions::little_endian;

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    TestCalc testCalc;
    TestMaterial testMaterial;
    TestConversions testConversions;

  return  QTest::qExec(&testCalc, argc, argv) |
          QTest::qExec(&testMaterial, argc, argv) |
          QTest::qExec(&testConversions, argc, argv);
}
