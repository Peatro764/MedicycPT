#include <QtTest>
#include "TestQARepo.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
//    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
//    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    TestQARepo testQARepo;

    return (QTest::qExec(&testQARepo, argc, argv));
}
