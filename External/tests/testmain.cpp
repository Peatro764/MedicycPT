#include <QtTest>
#include "TestQCPCurveUtils.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    TestQCPCurveUtils testQCPCurveUtils;

  return QTest::qExec(&testQCPCurveUtils, argc, argv);
}
