#include <QtTest>
#include "TestCollimator.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
//    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
//    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    TestCollimator testCollimator;

    return QTest::qExec(&testCollimator, argc, argv); // |
}
