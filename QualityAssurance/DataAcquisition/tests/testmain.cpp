#include <QtTest>

#include "TestDeviceManager.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    TestDeviceManager testDeviceManager;

    return QTest::qExec(&testDeviceManager, argc, argv);
}
