#include "TestSeancePacket.h"

void TestSeancePacket::initTestCase() {}

void TestSeancePacket::cleanupTestCase() {}

void TestSeancePacket::init() {}

void TestSeancePacket::cleanup() {}

QByteArray TestSeancePacket::GetByteArray() const {
    QByteArray expByteArray;
    expByteArray.resize(35);

    expByteArray[0] = 0x86;     // 78726
    expByteArray[1] = 0x33;
    expByteArray[2] = 0x01;
    expByteArray[3] = 0x00;
    expByteArray[4] = 0x01; //seanceid
    expByteArray[5] = 0x00;
    expByteArray[6] = 0x03; // cf9status
    expByteArray[7] = 0x30; // 99888
    expByteArray[8] = 0x86;
    expByteArray[9] = 0x01;
    expByteArray[10] = 0x00;
    expByteArray[11] = 0x66; // ct1 343654 nA
    expByteArray[12] = 0x3e;
    expByteArray[13] = 0x05;
    expByteArray[14] = 0x00;
    expByteArray[15] = 0x9a; // ct2 9.-343654 nA
    expByteArray[16] = 0xc1;
    expByteArray[17] = 0xfa;
    expByteArray[18] = 0xff;
    expByteArray[19] = 0xff; // stripper 2147483647 nA
    expByteArray[20] = 0xff;
    expByteArray[21] = 0xff;
    expByteArray[22] = 0x7f;
    expByteArray[23] = 0x01; // cf9 -2147483647 nA
    expByteArray[24] = 0x00;
    expByteArray[25] = 0x00;
    expByteArray[26] = 0x80;
    expByteArray[27] = 0x20; //32
    expByteArray[28] = 0x00;
    expByteArray[29] = 0x00;
    expByteArray[30] = 0x00;
    expByteArray[31] = 0x04; // 4
    expByteArray[32] = 0x00;
    expByteArray[33] = 0x00;
    expByteArray[34] = 0x00;

    return expByteArray;
}

void TestSeancePacket::TestByteArray() {
    SeancePacket seance_info(78726, 1, 3, 99888, 343654, -343654, 2147483647, -2147483647, 32, 4);

    QByteArray asByteArray = seance_info.ByteArray();
    QByteArray expByteArray = GetByteArray();

    QCOMPARE(asByteArray, expByteArray);
}

void TestSeancePacket::TestUnPack() {
    QByteArray ba = GetByteArray();
    SeancePacket seance_info = SeancePacket::UnPack(ba);
    SeancePacket exp_seance_info(78726, 1, 3, 99888, 343654, -343654, 2147483647, -2147483647, 32, 4);
    QCOMPARE(seance_info, exp_seance_info);
}

void TestSeancePacket::TestEquality() {
    SeancePacket s1(78726, 1, 3, 99888, 343654, -343654, 2147483647, -2147483647, 32, 4);
    SeancePacket s2(78727, 1, 3, 99888, 343654, -343654, 2147483647, -2147483647, 32, 4);
    SeancePacket s3(78726, 2, 3, 99888, 343654, -343654, 2147483647, -2147483647, 32, 4);
    SeancePacket s5(78726, 1, 5, 99888, 343654, -343654, 2147483647, -2147483647, 32, 4);
    SeancePacket s6(78726, 1, 3, 99889, 343654, -343654, 2147483647, -2147483647, 32, 4);
    SeancePacket s7(78726, 1, 3, 99888, 343653, -343654, 2147483647, -2147483647, 32, 4);
    SeancePacket s8(78726, 1, 3, 99888, 343654, -343653, 2147483647, -2147483647, 32, 4);
    SeancePacket s9(78726, 1, 3, 99888, 343654, -343654, 2147483646, -2147483647, 32, 4);
    SeancePacket s10(78726, 1, 3, 99888, 343654, -343654, 2147483647, -2147483646, 32, 4);
    SeancePacket s11(78726, 1, 3, 99888, 343654, -343654, 2147483647, -2147483647, 33, 4);
    SeancePacket s12(78726, 1, 3, 99888, 343654, -343654, 2147483647, -2147483647, 32, 5);

    QVERIFY(s1 == s1);
    QVERIFY(s1 != s2);
    QVERIFY(s1 != s3);
    QVERIFY(s1 != s5);
    QVERIFY(s1 != s6);
    QVERIFY(s1 != s7);
    QVERIFY(s1 != s8);
    QVERIFY(s1 != s9);
    QVERIFY(s1 != s10);
    QVERIFY(s1 != s11);
    QVERIFY(s1 != s12);
}

void TestSeancePacket::TestSameSeance() {
    SeancePacket s1(78726, 1, 3, 99888, 343654, -343654, 2147483647, -2147483647, 32, 4);
    SeancePacket s2(78725, 1, 3, 99888, 343654, -343654, 2147483647, -2147483647, 32, 4);
    SeancePacket s3(78726, 2, 3, 99888, 343654, -343654, 2147483647, -2147483647, 32, 4);

    QVERIFY(s1.SameSeance(s1));
    QVERIFY(!s1.SameSeance(s2));
    QVERIFY(!s1.SameSeance(s3));
}
