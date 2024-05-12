#include <QtTest>
#include "TestUtil.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    TestUtil testUtil;

    return  QTest::qExec(&testUtil, argc, argv);
}
