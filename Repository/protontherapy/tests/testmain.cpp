#include <QtTest>
#include "TestPTRepo.h"
#include "TestDbUtil.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
//    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
//    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    TestPTRepo testPTRepo;
    TestDbUtil testDbUtil;

    return (QTest::qExec(&testPTRepo, argc, argv) |
            QTest::qExec(&testDbUtil, argc, argv));
}
